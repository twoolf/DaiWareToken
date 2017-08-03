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
const cache = require('./cache');

async function addUsersFromGroup(stormpathGroupId) {
  const oktaGroupId = cache.groupMap[stormpathGroupId];
  const accountIds = cache.groupMembershipMap[stormpathGroupId];
  if (!accountIds || accountIds.length === 0) {
    return;
  }
  logger.info(`Adding ${accountIds.length} users to Okta group id=${oktaGroupId}`);
  const missing = cache.unifiedAccounts.getMissingAccounts(accountIds);
  for (let accountId of missing) {
    logger.error(`No Okta user for Stormpath accountId=${accountId}, skipping map to Okta groupId=${oktaGroupId}`);
  }
  const userIds = cache.unifiedAccounts.getUserIdsByAccountIds(accountIds);
  cache.groupUserMap[stormpathGroupId] = userIds;
  return addUsersToGroup(oktaGroupId, userIds);
}

module.exports = addUsersFromGroup;
