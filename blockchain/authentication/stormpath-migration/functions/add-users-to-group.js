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
const { each } = require('../util/concurrency');
const ApiError = require('../util/api-error');
const logger = require('../util/logger');
const rs = require('../util/request-scheduler');
const config = require('../util/config');

async function addUsersToGroup(groupId, userIds) {
  return each(userIds, async (userId) => {
    try {
      await rs.put({ url: `/api/v1/groups/${groupId}/users/${userId}` });
      logger.created(`Group Membership uid=${userId} gid=${groupId}`);
    } catch (err) {
      logger.error(new ApiError(`Failed to add uid=${userId} to gid=${groupId}`, err));
    }
  }, config.concurrencyLimit);
}

module.exports = addUsersToGroup;
