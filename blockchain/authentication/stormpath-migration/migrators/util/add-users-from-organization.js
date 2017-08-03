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
const addUsersToGroup = require('../../functions/add-users-to-group');
const logger = require('../../util/logger');
const stormpathExport = require('../../stormpath/stormpath-export');
const config = require('../../util/config');
const cache = require('./cache');

async function getOrganizationAccountStoreMap() {
  const mappings = await stormpathExport.getOrganizationAccountStoreMappings();
  return mappings.mapToObject((mapping, map) => {
    const type = mapping.accountStoreType;
    const id = mapping.accountStoreId;
    let userIds;
    switch (type) {
    case 'groups':
      userIds = cache.groupUserMap[id];
      break;
    case 'directories':
      userIds = cache.directoryUserMap[id];
      break;
    default:
      throw new Error(`Unknown organization account store mapping type: ${type}`);
    }

    const orgId = mapping.organization.id;
    if (!map[orgId]) {
      map[orgId] = [];
    }
    if (Array.isArray(userIds) && userIds.length > 0) {
      map[orgId] = map[orgId].concat(userIds);
    }
  }, { limit: config.concurrencyLimit });
}

async function addUsersFromOrganization(orgId) {
  if (!cache.organizationAccountStoreMappings) {
    cache.organizationAccountStoreMap = await getOrganizationAccountStoreMap();
  }

  const groupId = cache.organizationMap[orgId];
  const userIds = cache.organizationAccountStoreMap[orgId];

  if (!userIds || userIds.length === 0) {
    return;
  }

  logger.info(`Adding ${userIds.length} users to Group id=${groupId}`);
  return addUsersToGroup(groupId, userIds);
}

module.exports = addUsersFromOrganization;
