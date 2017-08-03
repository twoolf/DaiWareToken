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
const fs = require('fs');
const generator = require('generate-password');
const Base = require('./base');
const logger = require('../util/logger');
const config = require('../util/config');
const convertMCF = require('../util/convert-mcf');
const cache = require('../migrators/util/cache');

/**
 * Flattens custom data object, i.e:
 *
 * {
 *   "address": {
 *     "street": "1st ave",
 *     "zip": 1234
 *   },
 *   "freeForm": {},
 *   "hello": "world",
 *   "memos": [
 *     "one",
 *     "two",
 *     "three"
 *   ],
 *   "nested": {
 *     "again": {
 *       "yolo": "swag"
 *     }
 *   },
 *   "preferences": {
 *     "theme": "blue",
 *     "columns": [ "date", "time" ]
 *   },
 *   "favoriteNumbers": [ 7, 13 , 21],
 *   "emptySet": [],
 *   "mixedTypes1": [ 1, "a" ],
 *   "mixedTypes2": [ "b", 2 ],
 *   "someObjects": [ { "option1" : "foo" }, { "option2" : "foo" }]
 * }
 *
 * Becomes:
 *
 * {
 *    "address_street": "1st ave",
 *    "address_zip": 1234,
 *    "hello": "world",
 *    "memos": [
 *      "one",
 *      "two",
 *      "three"
 *    ],
 *    "nested_again_yolo": "swag",
 *    "preferences_theme": "blue",
 *    "preferences_columns": [
 *      "date",
 *      "time"
 *    ],
 *    "favoriteNumbers": [
 *      7,
 *      13,
 *      21
 *    ],
 *    "emptySet": [],
 *    "mixedTypes1": [
 *      "1",
 *      "a"
 *    ],
 *    "mixedTypes2": [
 *      "b",
 *      "2"
 *    ],
 *    "someObjects": [
 *      "{\"option1\":\"foo\"}",
 *      "{\"option2\":\"foo\"}"
 *    ]
 * }
 */
function flattenCustomData(customData, prefix = '') {
  const keys = Object.keys(customData);
  const prefixStr = prefix === '' ? '' : `${prefix}_`;
  const flattened = {};
  for (let key of keys) {
    const val = customData[key];
    if (!!val && !Array.isArray(val) && typeof val === 'object') {
      const nested = flattenCustomData(val, `${prefixStr}${key}`);
      Object.assign(flattened, nested);
    }
    else {
      flattened[`${prefixStr}${key}`] = val;
    }
  }
  return flattened;
}

/**
 * Transforms custom data value to an object with:
 *   type: array-number, array-string, boolean, number, string
 *   val: coerced value
 * If the type is an object, stringifies the object and stores as a string.
 * @param {*} val custom data value
 * @return {Object} type, val
 */
function transform(original) {
  let type;
  let val;

  if (Array.isArray(original)) {
    // There are three array types - string, number, and integer. If the array
    // is empty, or its first value is anything other than a number, use
    // the string array.

    const typesAreSame = original.reduce((set, val) => set.add(typeof val), new Set()).size <= 1;

    type = (original.length > 0) && (typeof original[0] === 'number') && typesAreSame ? 'array-number' : 'array-string';

    val = original.map((item) => {
      return type === 'array-number' ? item : (typeof item === 'string' ? item : JSON.stringify(item));
    });
  }
  else if (typeof original === 'boolean') {
    type = 'boolean';
    val = original;
  }
  else if (typeof original === 'number') {
    type = 'number';
    val = original;
  }
  else if (typeof original === 'string') {
    type = 'string';
    val = original;
  }
  else {
    type = 'string';
    val = JSON.stringify(original);
  }

  return { type, val };
}

/**
 * Sets default 'not_provided' value for required attributes
 * @param {Object} profileAttributes
 */
function addRequiredAttributes(profile) {
  const missing = [];
  ['firstName', 'lastName'].forEach((attr) => {
    if (!profile[attr]) {
      profile[attr] = 'not_provided';
      missing.push(attr);
    }
  });
  if (missing.length > 0) {
    const attrs = missing.join(',');
    logger.warn(`Setting required attributes ${attrs} to 'not_provided' for email=${profile.email}`);
  }
  return profile;
}

/**
 * Generates credentials with a random password
 */
function generateRandomPasswordCreds() {
  const password = generator.generate({
    length: 30,
    numbers: true,
    symbols: true,
    uppercase: true,
    strict: true
  });
  return { password: { value: password }};
}

/**
 * Creates creds object from an MCF formatted password. If the MCF identifier
 * is not Bcrypt or Stormpath, return a random password.
 *
 * Note: We are not currently going to support stormpath2, which is another
 * possible MCF identifier.
 *
 * @param {String} password MCF formatted password
 */
function transformMCFCreds(password, accountIds) {
  const hash = convertMCF(password);
  if (hash.algorithm !== 'BCRYPT' && hash.algorithm !== 'STORMPATH1') {
    logger.warn(`MCF identifier '${hash.algorithm}' is not supported, generating random password for accountId=${accountIds}`);
    return generateRandomPasswordCreds();
  }
  return {
    password: {
      hash
    },
    provider: {
      type: 'IMPORT',
      name: 'IMPORT'
    }
  };
}

class Account extends Base {

  initializeFromExport(options) {
    this.apiKeys = options.accountApiKeys[this.id] || [];
    this.accountIds = [this.id];
    this.directoryIds = [this.directory.id];

    this.externalIds = {};
    if (this.externalId) {
      this.externalIds[this.directory.id] = this.externalId;
    }

    this.recoveryAnswer = generator.generate({
      length: 30,
      numbers: true,
      uppercase: true,
      strict: true
    });
  }

  checkpointConfig() {
    const config = super.checkpointConfig();
    config.props = [
      'id',

      // Our props
      'apiKeys',
      'accountIds',
      'directoryIds',
      'externalIds',
      'recoveryAnswer',

      // Profile Attributes
      'username',
      'email',
      'givenName',
      'middleName',
      'surname',
      'fullName',
      'emailVerificationStatus',
      'href',
      'customData',
      'apiKeys',

      // Credentials
      'password',

      // Status
      'status'
    ];
    return config;
  }

  /**
   * Merges properties from another account into this account.
   * @param {Account} account
   */
  merge(account) {
    // 1. Base stormpath properties - only overrides properties that aren't already set
    const mergeableProperties = [
      'username',
      'givenName',
      'middleName',
      'surName',
      'fullName'
    ];
    mergeableProperties.forEach((prop) => {
      if (!this[prop]) {
        this[prop] = account[prop];
      }
    });

    // 2. Custom data properties - only overrides properties that aren't already set
    Object.keys(account.customData).forEach((key) => {
      if (!this.customData[key]) {
        this.customData[key] = account.customData[key];
      }
    });

    // 3. ApiKeys - merges both apiKeys together
    this.apiKeys = this.apiKeys.concat(account.apiKeys);

    // 4. Keep a record of which accounts have been merged
    this.accountIds.push(account.id);
    this.directoryIds.push(account.directory.id);

    // 5. Add directoryId -> externalId mapping if there is an externalId
    if (account.externalId) {
      this.externalIds[account.directory.id] = account.externalId;
    }
  }

  getStatus() {
    return this.status === 'DISABLED' ? 'SUSPENDED' : 'ACTIVE';
  }

  getProfileAttributes() {
    // Note: firstName and lastName are required attributes. If these are not
    // available, default to "not_provided"
    const profileAttributes = addRequiredAttributes({
      login: this.username,
      email: this.email,
      firstName: this.givenName,
      middleName: this.middleName,
      lastName: this.surname,
      displayName: this.fullName,
      emailVerificationStatus: this.emailVerificationStatus
    });

    profileAttributes.stormpathHref = this.href;

    const customData = this.getCustomData();
    const invalid = [];
    Object.keys(customData).forEach((key) => {
      const property = customData[key];
      const schemaType = cache.customSchemaTypeMap[key];
      if (property.type !== schemaType) {
        invalid.push({ property: key, type: property.type, expected: schemaType });
      }
      else {
        profileAttributes[key] = customData[key].val;
      }
    });

    if (invalid.length > 0) {
      logger.warn(`Account ids=${this.accountIds} contain customData that does not match the expected schema types - removing`, invalid);
    }

    return profileAttributes;
  }

  getCredentials() {
    let creds;
    if (!this.password) {
      // If there is no password, generate a random temporary password so that
      // no activation email is sent.
      logger.warn(`No password set, generating random password for accountId=${this.accountIds}`);
      creds = generateRandomPasswordCreds();
    } else {
      creds = transformMCFCreds(this.password, this.accountIds);
    }

    creds.recovery_question = {
      question: 'Stormpath recovery answer',
      answer: this.recoveryAnswer
    };

    return creds;
  }

  getCustomData() {
    const customData = {};

    if (config.isCustomDataStringify) {
      customData['customData'] = transform(JSON.stringify(this.customData));
    }
    else if (config.isCustomDataFlatten) {
      const skip = ['createdAt', 'modifiedAt', 'href', 'id'];
      const flattened = flattenCustomData(this.customData);
      const keys = Object.keys(flattened).filter(key => !skip.includes(key));
      for (let key of keys) {
        // We store apiKeys/secrets under the stormpathApiKey_ namespace, throw
        // an error if they try to create a custom property with this key
        if (key.indexOf('stormpathApiKey_') === 0) {
          throw new Error(`${key} is a reserved property name`);
        }
        customData[key] = transform(flattened[key]);
      }
    }

    // Add apiKeys to custom data with the special keys stormpathApiKey_*
    this.apiKeys.forEach((key, i) => {
      if (i < 10) {
        customData[`stormpathApiKey_${i+1}`] = transform(`${key.id}:${key.secret}`);
      }
    });
    const numApiKeys = this.apiKeys.length;
    if (numApiKeys > 10) {
      logger.warn(`Account id=${this.id} has ${numApiKeys} apiKeys, but max is 10. Dropping ${numApiKeys - 10} keys.`);
    }

    // Add recovery question answer
    customData['stormpathMigrationRecoveryAnswer'] = transform(this.recoveryAnswer);

    return customData;
  }

  getExternalIdForDirectory(directoryId) {
    return this.externalIds[directoryId];
  }

}

module.exports = Account;
