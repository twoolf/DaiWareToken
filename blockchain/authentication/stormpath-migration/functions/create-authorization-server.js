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

const AS_PATH = '/api/v1/authorizationServers';

async function getExistingAuthorizationServer(name) {
  logger.verbose(`Getting existing authorization server name=${name}`);
  try {
    const authorizationServers = await rs.get({
      url: AS_PATH,
      qs: {
        q: name
      }
    });
    return authorizationServers.find(as => as.name === name);
  } catch (err) {
    throw new ApiError(`Failed to get authorization servers`, err);
  }
}

async function updateExistingAuthorizationServer(as) {
  logger.exists(`Found matching Authorization Server id=${as.id} name=${as.name}`);
  return as;
}

async function createNewAuthorizationServer(name, description, defaultResourceUri) {
  logger.verbose(`Creating authorization server with name=${name}`);
  try {
    const as = await rs.post({
      url: AS_PATH,
      body: {
        name,
        description,
        audiences: [defaultResourceUri],
      }
    });
    logger.created(`AuthorizationServer id=${as.id} name=${name}`);
    return as;
  } catch (err) {
    throw new Error(`Failed to create authorization server name=${name}: ${err}`);
  }
}

async function createAuthorizationServer(name, description, defaultResourceUri) {
  logger.verbose(`Trying to create authorization server name=${name}`);
  const as = await getExistingAuthorizationServer(name);
  return as
    ? updateExistingAuthorizationServer(as)
    : createNewAuthorizationServer(name, description, defaultResourceUri);
}

module.exports = createAuthorizationServer;
