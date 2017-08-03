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

const CLIENTS_PATH = '/oauth2/v1/clients';

async function getOAuthClient(name) {
  logger.verbose(`Getting existing OAuth Client client_name=${name}`);
  const clients = await rs.get({
    url: CLIENTS_PATH,
    qs: {
      q: name
    }
  });
  const exactMatches = clients.filter(client => client.client_name === name);
  if (exactMatches.length > 1) {
    throw new Error(`Found too many OAuth Clients matching client_name=${name}`);
  }
  return exactMatches.length === 1 ? exactMatches[0] : null;
}

async function updateOAuthClient(client) {
  logger.exists(`Found matching OAuth Client client_id=${client.client_id} client_name=${client.client_name}`);
  return client;
}

async function createNewOAuthClient(name) {
  logger.verbose(`No OAuth clients found with client_name=${name}`);
  try {
    const client = await rs.post({
      url: CLIENTS_PATH,
      body: {
        client_name: name,
        response_types: ['code', 'token', 'id_token'],
        grant_types: [
          'authorization_code',
          'implicit',
          'password',
          'refresh_token'
        ],
        redirect_uris: ['https://www.okta.com/redirect-not-provided'],
        token_endpoint_auth_method: 'client_secret_basic',
        application_type: 'web'
      }
    });
    logger.created(`OAuth Client client_id=${client.client_id} client_name=${name}`);
    return client;
  } catch (err) {
    throw new ApiError(`Failed to create OAuth Client client_name=${name}`, err);
  }
}

async function createOAuthClient(name) {
  logger.verbose(`Trying to create oauth client name=${name}`);
  const client = await getOAuthClient(name);
  return client ? updateOAuthClient(client) : createNewOAuthClient(name);
}

module.exports = createOAuthClient;
