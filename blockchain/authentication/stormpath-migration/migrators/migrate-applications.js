/*!
 * Copyright (c) 2017, Okta, Inc. and/or its affiliates. All rights reserved.
 * The Okta software accompanied by this notice is provided pursuant to the Apache License, Version 2.0 (the "License.")
 *
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 * See the License for the specific language governing permissions and limitations under the License.
 */
const Promise = require('bluebird');
const createOAuthClient = require('../functions/create-oauth-client');
const createAuthorizationServer = require('../functions/create-authorization-server');
const addAuthorizationServerToOAuthClient = require('../functions/add-authorization-server-to-oauth-client');
const createDefaultResourceAccessPolicy = require('../functions/create-default-resource-access-policy');
const stormpathExport = require('../stormpath/stormpath-export');
const logger = require('../util/logger');
const allSettled = require('../util/all-settled');
const config = require('../util/config');
const addGroupsToApp = require('../functions/add-groups-to-app');
const cache = require('./util/cache');

function getAppIdFromClient(client) {
  const parts = client._links.app.href.split('/');
  return parts[parts.length - 1];
}

async function getAccountStoreMap() {
  const mappings = await stormpathExport.getAccountStoreMappings();
  logger.info(`Processing ${mappings.length} account store mappings`);
  return mappings.mapToObject((mapping, map) => {
    let oktaGroupId;
    const type = mapping.accountStoreType;
    const id = mapping.accountStoreId;
    switch (type) {
    case 'groups':
      oktaGroupId = cache.groupMap[id];
      break;
    case 'organizations':
      oktaGroupId = cache.organizationMap[id];
      break;
    case 'directories':
      oktaGroupId = cache.directoryMap[id];
      break;
    default:
      throw new Error(`Unknown account store mapping type: ${type}`);
    }

    const appId = mapping.application.id;
    if (!map[appId]) {
      map[appId] = [];
    }
    if (!oktaGroupId) {
      logger.error(`No Okta group for Stormpath accountStoreMappingId=${mapping.id}`);
    } else {
      map[appId].push(oktaGroupId);
    }
  }, { limit: config.concurrencyLimit });
}

async function addGroupsToApplication(application, client) {
  const appInstanceId = client.client_id ? client.client_id : getAppIdFromClient(client);

  const groupIds = cache.accountStoreMap[application.id];
  if (!groupIds || groupIds.length === 0) {
    return logger.error('No Okta groups found to assign to application');
  }
  logger.info(`Adding application id=${appInstanceId} to ${groupIds.length} groups`);
  return addGroupsToApp(appInstanceId, groupIds);
}

async function migrateApplication(application) {
  const lg = logger.group(`Stormpath application id=${application.id} name=${application.name}`);
  try {
    const name = `app:${application.name}`;
    const description = `Imported from Stormpath application id=${application.id}`;
    const [client, as] = await allSettled([
      createOAuthClient(name),
      createAuthorizationServer(name, description, application.href)
    ]);
    await addGroupsToApplication(application, client);
    await allSettled([
      createDefaultResourceAccessPolicy(as, client, application.tokenLimits),
      addAuthorizationServerToOAuthClient(as, client)
    ]);
  } catch (err) {
    logger.error(err);
    if (err.message.includes('Maximum number of instances has been reached')) {
      throw new Error('Reached maximum number of OAuth applications - contact support to raise this limit');
    }
  } finally {
    lg.end();
  }
}

async function migrateApplications() {
  logger.header('Starting applications import');
  cache.accountStoreMap = await getAccountStoreMap();

  const applications = await stormpathExport.getApplications();

  logger.info(`Importing ${applications.length} applications`);

  try {
    await applications.each(migrateApplication, { limit: 1 });
  } catch (err) {
    logger.error(`Failed to import all applications: ${err.message}`);
  }
}

module.exports = migrateApplications;
