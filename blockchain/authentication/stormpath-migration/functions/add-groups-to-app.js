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
const config = require('../util/config');
const rs = require('../util/request-scheduler');
const logger = require('../util/logger');

async function addGroupsToApp(appInstanceId, groupIds) {
  logger.verbose(`Adding groupIds=${groupIds} to appInstanceId=${appInstanceId}`);
  return each(groupIds, async (groupId) => {
    logger.verbose(`Adding groupId=${groupId} to appInstanceId=${appInstanceId}`);
    try {
      await rs.put({ url: `/api/v1/apps/${appInstanceId}/groups/${groupId}` });
      logger.created(`Assigned groupId=${groupId} to appInstanceId=${appInstanceId}`);
    } catch (err) {
      logger.error(new ApiError(`Failed to assign groupId=${groupId} to appInstanceId=${appInstanceId}`, err));
    }
  }, config.concurrencyLimit);
}

module.exports = addGroupsToApp;
