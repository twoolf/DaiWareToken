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
const ApiError = require('../util/api-error');
const logger = require('../util/logger');
const rs = require('../util/request-scheduler');
const config = require('../util/config');

function idpUsers(idpId) {
  return `/api/v1/idps/${idpId}/users`;
}

async function isLinked(userId, idpId) {
  logger.verbose(`Getting existing social link userId=${userId} idpId=${idpId}`);
  try {
    const users = await rs.get(idpUsers(idpId));
    return users.find(link => userId === link.id) ? true : false;
  } catch (err) {
    throw new ApiError(`Failed to get idps for userId=${userId}`, err);
  }
}

async function createLink(userId, idpId, externalId) {
  logger.verbose(`Creating new social link userId=${userId} idpId=${idpId} externalId=${externalId}`);
  try {
    await rs.post({
      url: `${idpUsers(idpId)}/${userId}`,
      body: { externalId }
    });
    logger.created(`Social link idpId=${idpId} userId=${userId} externalId=${externalId}`);
  } catch (err) {
    throw new ApiError(`Failed to link userId=${userId} idpId=${idpId}`, err);
  }
}

async function linkUserToSocialIdp(userId, idpId, externalId) {
  logger.verbose(`Trying to create social link idpId=${idpId} userId=${userId}`);
  const linkExists = await isLinked(userId, idpId);
  if (linkExists) {
    logger.exists(`Found matching social link idpId=${idpId} userId=${userId}`);
    return;
  }
  return createLink(userId, idpId, externalId);
}

module.exports = linkUserToSocialIdp;
