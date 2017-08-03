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
const APiError = require('../util/api-error');

const GROUPS_PATH = '/api/v1/groups';

async function getExistingGroup(name) {
  logger.verbose(`Getting existing Okta group name=${name}`);
  const groups = await rs.get({
    url: GROUPS_PATH,
    qs: {
      q: name
    }
  });
  const exactMatches = groups.filter(group => group.profile.name === name);
  if (exactMatches.length > 1) {
    throw new Error(`Found too many Okta groups matching name=${name}`);
  }
  return exactMatches.length === 1 ? exactMatches[0] : null;
}

async function updateExistingGroup(group, description) {
  logger.verbose(`Existing group found with name '${group.profile.name}' id=${group.id}`);

  if (description === group.profile.description) {
    logger.exists(`Found matching Okta group id=${group.id} name=${group.profile.name}`);
    return group;
  }

  try {
    Object.assign(group, {
      profile: {
        name: group.profile.name,
        description
      }
    });
    const updated = await rs.put({
      url: `${GROUPS_PATH}/${group.id}`,
      body: group
    });
    logger.updated(`Okta group id=${updated.id} name=${updated.profile.name}`);
    return updated;
  } catch (err) {
    throw new ApiError(`Failed to update Okta group name=${group.profile.name} id=${group.id}`, err);
  }
}

async function createNewGroup(name, description) {
  logger.verbose(`No Okta groups found with name=${name}`);
  try {
    const group = await rs.post({
      url: GROUPS_PATH,
      body: {
        profile: {
          name,
          description
        }
      }
    });
    logger.created(`Okta group id=${group.id} name=${name}`);
    return group;
  } catch (err) {
    throw new ApiError(`Failed to create Okta group name=${name}`, err);
  }
}

async function createOktaGroup(name, description) {
  logger.verbose(`Trying to create Okta group name=${name}`);
  const group = await getExistingGroup(name);
  return group
    ? updateExistingGroup(group, description)
    : createNewGroup(name, description);
}

module.exports = createOktaGroup;
