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
const logger = require('../util/logger');
const rs = require('../util/request-scheduler');
const ApiError = require('../util/api-error');

const APPS_PATH = '/api/v1/apps';

function getAppIdFromClient(client) {
  const parts = client._links.app.href.split('/');
  return parts[parts.length - 1];
}

async function addAuthorizationServerToOAuthClient(as, client) {
  const details = `Authorization Server id=${as.id} to OAuth Client client_id=${client.client_id}`;
  logger.verbose(`Trying to add ${details}`);
  try {
    const appId = client.client_id ? client.client_id : getAppIdFromClient(client);
    const app = await rs.get(`${APPS_PATH}/${appId}`);
    if (app.settings.notifications.vpn.message === as.id) {
      logger.exists(details);
      return;
    }
    logger.verbose('No map, creating');
    app.settings.notifications.vpn.message = as.id;
    const res = await rs.put({
      url: `${APPS_PATH}/${appId}`,
      body: app
    });
    logger.created(details);
  } catch (err) {
    throw new ApiError(`Failed to map ${details}`, err);
  }
}

module.exports = addAuthorizationServerToOAuthClient;
