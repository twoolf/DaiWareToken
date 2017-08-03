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
const Promise = require('bluebird');
const parseIsoDuration = require('parse-iso-duration');
const fs = require('fs');
const readFile = Promise.promisify(fs.readFile);
const Base = require('./base');
const logger = require('../util/logger');
const config = require('../util/config');

function warn(policy, msg) {
  logger.warn(`OAuth policy id=${policy.id}: ${msg}`);
}

function isoToMin(isoDuration) {
  const ms = parseIsoDuration(isoDuration);
  return Math.floor(ms / 1000 / 60);
}

async function loadOAuthPolicy(application) {
  const oktaPolicy = {
    accessTokenLifetimeMinutes: 60, // 1 hour
    refreshTokenLifetimeMinutes: 144000, // 100 days

    // Expires the refresh token if it is not used in X minutes. Since this is
    // not a Stormpath feature, set to 0 (unlimited).
    refreshTokenWindowMinutes: 0
  };

  if (!application.oAuthPolicy) {
    logger.verbose(`No oAuthPolicy for applicationId=${application.id}`);
    return oktaPolicy;
  }

  const policyId = application.oAuthPolicy.id;
  const filePath = `${config.stormPathBaseDir}/oAuthPolicies/${policyId}.json`;

  try {
    logger.verbose(`Loading oAuthPolicy id=${policyId} for applicationId=${application.id}`);
    const content = await readFile(filePath, 'utf8');
    const policy = JSON.parse(content);

    if (policy.idTokenTtL !== 'PT0H' && policy.idTokenTtl !== 'PT1H') {
      warn(policy, 'Okta ID Token lifetime is fixed to 1 hour');
    }

    oktaPolicy.accessTokenLifetimeMinutes = isoToMin(policy.accessTokenTtl);
    oktaPolicy.refreshTokenLifetimeMinutes = isoToMin(policy.refreshTokenTtl);
    return oktaPolicy;
  } catch (err) {
    logger.error(`Failed to read oAuthPolicy id=${policyId} for applicationId=${application.id}: ${err}`);
    return oktaPolicy;
  }
}

class Application extends Base {

  async initializeFromExport() {
    this.tokenLimits = await loadOAuthPolicy(this);
  }

}

module.exports = Application;
