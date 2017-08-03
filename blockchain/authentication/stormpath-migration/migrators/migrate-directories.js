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
const logger = require('../util/logger');
const rs = require('../util/request-scheduler');
const stormpathExport = require('../stormpath/stormpath-export');
const createOktaGroup = require('../functions/create-okta-group');
const createGroupPasswordPolicy = require('../functions/create-group-password-policy');
const createSocialIdp = require('../functions/create-social-idp');
const createSamlIdp = require('../functions/create-saml-idp');
const addUsersFromDirectory = require('./util/add-users-from-directory');
const linkUsersFromSocialDirectory = require('./util/link-users-from-social-directory');
const addAndMapIdpAttributes = require('../functions/add-and-map-idp-attributes');
const config = require('../util/config');
const cache = require('./util/cache');

function directoryDescription(directory) {
  let description = `Stormpath directoryId=${directory.id}`;
  if (directory.description) {
    description += `: ${directory.description}`;
  }
  return description;
}

async function migrateCloud(directory) {
  const name = `dir:${directory.name}`;
  const description = directoryDescription(directory);
  const group = await createOktaGroup(name, description);
  cache.directoryMap[directory.id] = group.id;
  if (directory.passwordPolicy) {
    await createGroupPasswordPolicy(group.id, directory.passwordPolicy);
  }
  return addUsersFromDirectory(directory.id);
}

async function migrateSocial(type, directory) {
  const name = `dir:${directory.name}`;
  const description = directoryDescription(directory);
  const group = await createOktaGroup(name, description);
  cache.directoryMap[directory.id] = group.id;
  const provider = directory.provider;
  const idp = await createSocialIdp({
    type: type,
    name,
    groupId: group.id,
    creds: {
      clientId: provider.clientId,
      clientSecret: provider.clientSecret
    },
    scopes: provider.scope
  });
  await addAndMapIdpAttributes(idp.id, directory.attributeMappings);
  cache.directoryIdpMap[directory.id] = idp.id;
  await addUsersFromDirectory(directory.id);
  return linkUsersFromSocialDirectory(directory.id);
}

async function migrateSaml(directory) {
  const provider = directory.provider;
  let requestAlgorithm;
  switch (provider.requestSignatureAlgorithm) {
  case 'RSA-SHA256':
    requestAlgorithm = 'SHA-256';
    break;
  case 'RSA-SHA1':
    requestAlgorithm = 'SHA-1';
    break;
  default:
    logger.error(`Invalid request algorithm: ${provider.requestSignatureAlgorithm}`);
    return;
  }
  const idp = await createSamlIdp({
    signingCert: directory.signingCert,
    name: `dir:${directory.name}`,
    requestSignatureAlgorithm: requestAlgorithm,
    ssoLoginUrl: provider.ssoLoginUrl
  });
  return addAndMapIdpAttributes(idp.id, directory.attributeMappings);
}

async function migrateDirectory(directory) {
  const lg = logger.group(`Stormpath directory id=${directory.id} name=${directory.name}`);
  try {
    if (directory.status !== 'ENABLED') {
      return logger.warn(`Skipping directory id=${directory.id} - not enabled`);
    }
    const provider = directory.provider.providerId;
    switch (provider) {
    case 'stormpath':
      return await migrateCloud(directory);
    case 'saml':
      return await migrateSaml(directory);
    case 'facebook':
      return await migrateSocial('FACEBOOK', directory);
    case 'google':
      return await migrateSocial('GOOGLE', directory);
    case 'linkedin':
      return await migrateSocial('LINKEDIN', directory);
    case 'ad':
    case 'ldap':
      // We should include a link to some documentation they can use to setup
      // the AD agent the run the import.
      logger.warn(`${provider.toUpperCase()} directories must be imported with the Okta agent`);
      return;
    default:
      // github, twitter
      logger.warn(`We do not support migrating the '${provider}' directory type`);
      return;
    }
  } catch (err) {
    logger.error(err);
  } finally {
    lg.end();
  }
}

async function migrateDirectories() {
  logger.header('Starting directories import');
  try {
    const directories = await stormpathExport.getDirectories();
    logger.info(`Importing ${directories.length} directories`);
    await directories.each(migrateDirectory, { limit: 1 });
  } catch (err) {
    logger.error(err);
  }
}

module.exports = migrateDirectories;
