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

async function getDefaultPolicy(as) {
  logger.verbose(`Getting default policy for asId=${as.id}`);
  try {
    const policies = await rs.get({
      url: `${AS_PATH}/${as.id}/policies`,
      query: {
        type: 'OAUTH_AUTHORIZATION_POLICY'
      }
    });
    return policies.find(policy => policy.name === 'Default Policy');
  } catch (err) {
    throw new ApiError(`Failed to get default policy for asId=${as.id}`, err);
  }
}

async function createDefaultPolicy(as, client) {
  logger.verbose(`Creating default resource policy for asId=${as.id}`);
  try {
    const policy = await rs.post({
      url: `${AS_PATH}/${as.id}/policies`,
      body: {
        name: 'Default Policy',
        type: 'OAUTH_AUTHORIZATION_POLICY',
        conditions: {
          clients: {
            include: [client.client_id]
          }
        }
      }
    });
    logger.created(`Default policy id=${policy.id} for asId=${as.id}`);
    return policy;
  } catch (err) {
    throw new ApiError(`Failed to create default resource policy for asId=${as.id}`, err);
  }
}

async function getDefaultRule(as, policy) {
  logger.verbose(`Getting default rule for asId=${as.id}policyId=${policy.id}`);
  try {
    const rules = await rs.get(`${AS_PATH}/${as.id}/policies/${policy.id}/rules`);
    return rules.find(rule => rule.name === 'Default Rule');
  } catch (err) {
    throw new ApiError(`Failed to get default rule for asId=${as.id} policyId=${policy.id}`, err);
  }
}

async function updateDefaultRule(as, policy, rule, tokenLimits) {
  logger.verbose(`Updating existing default policy rule id=${rule.id}`);
  try {
    // Only token limits can be updated (i.e. accessTokenLifetimeMinutes, etc)
    rule.actions.token = tokenLimits;
    const updated = await rs.put({
      url: `${AS_PATH}/${as.id}/policies/${policy.id}/rules/${rule.id}`,
      body: rule
    });
    logger.updated(`Default policy rule id=${rule.id} for asId=${as.id} policyId=${policy.id}`);
    return updated;
  } catch (err) {
    throw new ApiError(`Failed to update default authorization policy rule id=${rule.id}`, err);
  }
}

async function createDefaultRule(as, policy, tokenLimits) {
  logger.verbose(`Creating default resource policy rule for asId=${as.id} policyId=${policy.id}`);
  try {
    const rule = await rs.post({
      url: `${AS_PATH}/${as.id}/policies/${policy.id}/rules`,
      body: {
        name: 'Default Rule',
        type: 'RESOURCE_ACCESS',
        status: 'ACTIVE',
        system: false,
        actions: {
          token: tokenLimits
        },
        conditions: {
          grantTypes: {
            include: [
              'authorization_code',
              'password'
            ]
          },
          people: {
            users: {
              include: [],
              exclude: []
            },
            groups: {
              include: ['EVERYONE'],
              exclude: []
            }
          },
          scopes: {
            include: [
              '*'
            ]
          },
        }
      }
    });
    logger.created(`Default policy rule id=${rule.id} for asId=${as.id} policyId=${policy.id}`);
    return policy;
  } catch (err) {
    throw new ApiError(`Failed to create default authorization server policy rule for asId=${as.id} policyId=${policy.id}`, err);
  }
}

async function createDefaultResourceAccessPolicy(as, client, tokenLimits) {
  logger.verbose(`Trying to create default resource access policy for asId=${as.id} and clientId=${client.client_id}`);

  let defaultPolicy = await getDefaultPolicy(as);
  if (defaultPolicy) {
    logger.exists(`Found default policy id=${defaultPolicy.id} for asId=${as.id}`);
  } else {
    defaultPolicy = await createDefaultPolicy(as, client);
  }

  let defaultRule = await getDefaultRule(as, defaultPolicy);
  if (defaultRule) {
    await updateDefaultRule(as, defaultPolicy, defaultRule, tokenLimits);
  } else {
    await createDefaultRule(as, defaultPolicy, tokenLimits);
  }
}

module.exports = createDefaultResourceAccessPolicy;
