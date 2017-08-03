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

const IDP_PATH = '/api/v1/idps';

function getIdpJson(options) {
  return {
    type: options.type,
    name: options.name,
    status: 'ACTIVE',
    protocol: {
      type: 'OAUTH2',
      scopes: options.scopes,
      credentials: {
        client: {
          client_id: options.creds.clientId,
          client_secret: options.creds.clientSecret
        }
      }
    },
    policy: {
      provisioning: {
        action: 'AUTO',
        profileMaster: true,
        groups: {
          action: 'ASSIGN',
          assignments: [options.groupId]
        }
      },
      accountLink: {
        filter: null,
        action: 'AUTO'
      },
      subject: {
        userNameTemplate: {
          template: 'idpuser.email',
          type: null
        },
        filter: null,
        matchType: 'EMAIL'
      },
      maxClockSkew: 0
    }
  };
}

async function getExistingIdp(name) {
  logger.verbose(`GET existing Social IDP name=${name}`);
  const idps = await rs.get({
    url: IDP_PATH,
    qs: {
      q: name
    }
  });
  const exactMatches = idps.filter(idp => idp.name === name);
  return exactMatches.length > 0 ? exactMatches[0] : null;
}

async function updateExistingIdp(idp, json) {
  logger.verbose(`Updating existing Social IDP id=${idp.id} name=${json.name}`);
  try {
    Object.assign(idp, json);
    const updated = await rs.put({
      url: `${IDP_PATH}/${idp.id}`,
      body: idp
    });
    logger.updated(`Social IDP id=${idp.id} name=${updated.name}`);
    return updated;
  } catch (err) {
    throw new ApiError(`Failed to update Social IDP id=${idp.id} name=${json.name}`, err);
  }
}

async function createNewIdp(json) {
  logger.verbose(`Creating Social IDP name=${json.name}`);
  try {
    const created = await rs.post({
      url: IDP_PATH,
      body: json
    });
    logger.created(`Social IDP id=${created.id} name=${created.name}`);
    return created;
  } catch (err) {
    throw new ApiError(`Failed to create Social IDP name=${json.name}`, err);
  }
}

/**
 * Creates or updates Social IDP
 * @param {Object} options
 * @param {String} options.name
 * @param {String} options.type GOOGLE, FACEBOOK, LINKEDIN
 * @param {Object} options.creds
 * @param {String} options.creds.clientId
 * @param {String} options.creds.clientSecret
 * @param {Array} options.scopes
 */
async function createSocialIdp(options) {
  logger.verbose(`Trying to create Social IDP name=${options.name}`);
  const json = getIdpJson(options);
  const idp = await getExistingIdp(json.name);
  return idp
    ? updateExistingIdp(idp, json)
    : createNewIdp(json);
}

module.exports = createSocialIdp;
