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
const { each } = require('../../util/concurrency');
const linkUserToSocialIdp = require('../../functions/link-user-to-social-idp');
const logger = require('../../util/logger');
const cache = require('./cache');
const config = require('../../util/config');

function error(userId, idpId, msg) {
  logger.error(`Unable to link userId=${userId} to idpId=${idpId} - ${msg}`);
}

function linkUsersFromSocialDirectory(directoryId) {
  const userIds = cache.directoryUserMap[directoryId];
  const idpId = cache.directoryIdpMap[directoryId];

  if (!Array.isArray(userIds) || userIds.length === 0) {
    return;
  }

  logger.info(`Linking ${userIds.length} users to Social IdP id=${idpId}`);
  return each(userIds, async (userId) => {
    try {
      const accountRef = cache.userIdAccountMap[userId];
      if (!accountRef) {
        return error(userId, idpId, 'No unified account for user');
      }

      const account = await accountRef.getAccount();
      const externalId = account.getExternalIdForDirectory(directoryId);
      if (!externalId) {
        return error(userId, idpId, `No externalId found`);
      }
      await linkUserToSocialIdp(userId, idpId, externalId);
    } catch (err) {
      logger.error(err);
    }
  }, config.concurrencyLimit);
}

module.exports = linkUsersFromSocialDirectory;
