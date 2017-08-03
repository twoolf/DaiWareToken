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

const USERS_PATH = '/api/v1/users';

function getIdFromStormpathHref(href) {
  return href.substring(href.lastIndexOf('/') + 1);
}

async function getExistingUser(profile) {
  try {
    const users = await rs.get({
      url: USERS_PATH,
      qs: {
        filter: `profile.login eq "${profile.login}"`
      }
    });
    return users.length > 0 ? users[0] : null;
  } catch (err) {
    throw new ApiError('Failed to get existing users', err);
  }
}

// Note: We cannot update an imported password after the user is out of the
// STAGED status.
async function updateExistingUser(user, profile) {
  logger.verbose(`Updating existing user with login=${profile.login} id=${user.id}`);
  try {
    // We cannot update an imported password after user is ACTIVE
    delete user.credentials;
    // When updating the user, do not overwrite the recovery answer
    delete profile.stormpathMigrationRecoveryAnswer;

    Object.assign(user.profile, profile);
    const updated = await rs.post({
      url: `${USERS_PATH}/${user.id}`,
      body: user
    });
    const stormpathAccountId = getIdFromStormpathHref(profile.stormpathHref);
    logger.updated(`okta_user_id=${user.id}, login=${profile.login}, stormpath_account_id=${stormpathAccountId}`);
    return updated;
  } catch (err) {
    throw new ApiError(`Failed to update okta user id=${user.id} login=${profile.login}`, err);
  }
}

async function createNewUser(profile, credentials) {
  logger.verbose(`Creating user login=${profile.login}`);
  try {
    const user = await rs.post({
      url: `${USERS_PATH}?activate=false`,
      body: {
        profile,
        credentials
      }
    });
    const stormpathAccountId = getIdFromStormpathHref(profile.stormpathHref);
    logger.created(`okta_user_id=${user.id}, login=${profile.login}, stormpath_account_id=${stormpathAccountId}`);
    activate = await rs.post({
      url: `${USERS_PATH}/${user.id}/lifecycle/activate?sendEmail=false`
    });
    logger.info(`Activated User id=${user.id} login=${profile.login}`);
    return user;
  } catch (err) {
    throw new ApiError(`Failed to create User login=${profile.login}`, err);
  }
}

async function suspendUser(userId) {
  logger.verbose('Suspending user id=${userId}');
  try {
    await rs.post(`/api/v1/users/${userId}/lifecycle/suspend`);
    logger.info(`Suspended user id=${userId}`);
  } catch (err) {
    logger.error(new ApiError(`Failed to suspend user id=${userId}`, err));
  }
}

async function createOktaUser(profile, credentials, status) {
  logger.verbose(`Trying to create User login=${profile.login}`);
  const existing = await getExistingUser(profile);
  const user = existing
    ? await updateExistingUser(existing, profile)
    : await createNewUser(profile, credentials);

  if (user && user.status !== 'SUSPENDED' && status === 'SUSPENDED') {
    await suspendUser(user.id);
  }

  return user;
}

module.exports = createOktaUser;
