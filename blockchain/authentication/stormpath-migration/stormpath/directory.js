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
const fs = require('fs');
const readFile = Promise.promisify(fs.readFile);
const Base = require('./base');
const logger = require('../util/logger');
const config = require('../util/config');

function warn(policy, msg) {
  logger.warn(`Password policy id=${policy.id}: ${msg}`);
}

function mapToOktaPolicy(directory, policy) {
  const strength = policy.strength;
  if (strength.maxLength !== 100) {
    // Default value is 100. If they've changed it, warn them.
    warn(policy, 'maxLength policy is not supported');
  }
  if (strength.minLowerCase > 1) {
    warn(policy, 'Okta minLowerCase only requires at least 1 lowercase character');
    strength.minLowerCase = 1;
  }
  if (strength.minUpperCase > 1) {
    warn(policy, 'Okta minUpperCase only requires at least 1 uppercase character');
    strength.minUpperCase = 1;
  }
  if (strength.minNumeric > 1) {
    warn(policy, 'Okta minNumber only requires at least 1 number');
    strength.minNumeric = 1;
  }
  if (strength.minSymbol > 1) {
    warn(policy, 'Okta minSymbol only requires at least 1 symbol');
    strength.minSymbol = 1;
  }
  if (strength.minDiacritic > 0) {
    warn(policy, 'minDiacritic policy is not supported');
  }
  return {
    name: `${directory.name}-Policy`,
    description: `Imported from Stormpath passwordPolicy id=${policy.id}`,
    settings: {
      password: {
        complexity: {
          minLength: strength.minLength,
          minLowerCase: strength.minLowerCase,
          minUpperCase: strength.minUpperCase,
          minNumber: strength.minNumeric,
          minSymbol: strength.minSymbol,
          excludeUsername: false
        },
        age: {
          maxAgeDays: -1,
          expireWarnDays: 0,
          minAgeMinutes: -1,
          historyCount: strength.preventReuse
        },
        lockout: {
          maxAttempts: 10,
          autoUnlockMinutes: -1,
          showLockoutFailures: false
        }
      }
    }
  }
}

async function loadPasswordPolicy(directory) {
  if (!directory.passwordPolicy) {
    logger.verbose(`No passwordPolicy for directoryId=${directory.id}`);
    return;
  }
  const policyId = directory.passwordPolicy.id;
  const filePath = `${config.stormPathBaseDir}/passwordPolicies/${policyId}.json`;
  try {
    logger.verbose(`Loading passwordPolicy id=${policyId} for directoryId=${directory.id}`);
    const policy = await readFile(filePath, 'utf8');
    return mapToOktaPolicy(directory, JSON.parse(policy));
  } catch (err) {
    logger.error(`Failed to read passwordPolicy id=${policyId} for directoryId=${directory.id}: ${err}`);
  }
}

async function loadUserInfoMappingRules(directory) {
  const mappings = [];
  if (!directory.provider.userInfoMappingRules) {
    logger.verbose(`No userInfoMappingRules for directoryId=${directory.id}`);
    return mappings;
  }

  const filePath = `${config.stormPathBaseDir}/userInfoMappingRules/${directory.id}.json`;
  try {
    logger.verbose(`Loading userInfoMappingRules for directoryId=${directory.id}`);
    const content = await readFile(filePath, 'utf8');
    const rules = JSON.parse(content);
    for (let item of rules.items) {
      for (let attribute of item.accountAttributes) {
        // Account attributes are mapped to custom user schema properties by:
        // 1. Flattening, i.e. replacing '.' with '_'
        // 2. Removing the customData prefix, which is not in the user profile
        const userAttribute = attribute.replace('customData.', '').replace(/\./g, '_');
        mappings.push({
          externalName: item.name,
          userAttribute
        });
      }
    }
    return mappings;
  } catch (err) {
    logger.error(`Failed to read userInfoMappingRules for directoryId=${directory.id}: ${err}`);
  }

}

class Directory extends Base {

  async initializeFromExport() {
    this.passwordPolicy = await loadPasswordPolicy(this);
    this.attributeMappings = await loadUserInfoMappingRules(this);
    if (this.provider.providerId === 'saml') {
      this.signingCert = this.provider.encodedX509SigningCert
        .replace('-----BEGIN CERTIFICATE-----\n', '')
        .replace('\n-----END CERTIFICATE-----', '');
    }
  }

}

module.exports = Directory;
