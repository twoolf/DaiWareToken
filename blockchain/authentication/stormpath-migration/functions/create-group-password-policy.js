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

const POLICY_PATH = '/api/v1/policies';

// Cache the existing password policies since the list policies endpoint does
// now allow for filtering by name.
let passwordPolicies;

async function createDefaultRule(policy) {
  logger.verbose(`Trying to create default rule for password policy id=${policy.id}`);
  const url = `${POLICY_PATH}/${policy.id}/rules`;
  const existing = await rs.get(url);
  if (existing.length > 0) {
    logger.exists(`Found default rule for password policy id=${policy.id}`);
    return;
  }
  const created = await rs.post({
    url,
    body: {
      type: 'PASSWORD',
      name: 'Default Rule',
      conditions: {
        network: {
          connection: 'ANYWHERE'
        }
      },
      actions: {
        passwordChange: {
          access: 'ALLOW'
        },
        selfServicePasswordReset: {
          access: 'ALLOW'
        },
        selfServiceUnlock: {
          access: 'ALLOW'
        }
      }
    }
  });
  logger.created(`Password policy rule id=${created.id} name=${created.name}`);
}

function getPolicyJson(groupId, policy) {
  const json = {
    type: 'PASSWORD',
    conditions: {
      people: {
        groups: {
          include: [groupId]
        }
      }
    }
  };
  Object.assign(json, policy);
  return json;
}

async function getPasswordPolicy(name) {
  logger.verbose(`Getting existing passwordPolicy name=${name}`);
  if (!passwordPolicies) {
    logger.verbose('Getting and caching all existing password policies');
    try {
      const policies = await rs.get(`${POLICY_PATH}?type=PASSWORD`);
      passwordPolicies = {};
      for (let policy of policies) {
        passwordPolicies[policy.name] = policy;
      }
    } catch (err) {
      throw new ApiError(`Failed to get existing password policies name=${name}`, err);
    }
  }
  return passwordPolicies[name];
}

async function updatePasswordPolicy(existing, groupId, policy) {
  logger.verbose(`Updating existing password policy with name=${policy.name} id=${existing.id}`);
  try {
    Object.assign(existing, getPolicyJson(groupId, policy));
    const updated = await rs.put({
      url: `${POLICY_PATH}/${existing.id}`,
      body: existing
    });
    logger.updated(`Password policy id=${updated.id}`);
    await createDefaultRule(updated);
    return updated;
  } catch (err) {
    throw new ApiError(`Failed to update password policy id=${existing.id}`, err);
  }
}

async function createNewPasswordPolicy(groupId, policy) {
  logger.verbose(`Creating password policy name=${policy.name} for groupId=${groupId}`);
  try {
    const created = await rs.post({
      url: POLICY_PATH,
      body: getPolicyJson(groupId, policy)
    });
    logger.created(`Password policy id=${created.id} name=${created.name}`);
    await createDefaultRule(created);
    return created;
  } catch (err) {
    throw new ApiError(`Failed to create password policy name=${policy.name}`, err);
  }
}

async function createGroupPasswordPolicy(groupId, policy) {
  logger.verbose(`Trying to create password policy name=${policy.name} for groupId=${groupId}`);
  const existing = await getPasswordPolicy(policy.name);
  return existing
    ? updatePasswordPolicy(existing, groupId, policy)
    : createNewPasswordPolicy(groupId, policy);
}

module.exports = createGroupPasswordPolicy;
