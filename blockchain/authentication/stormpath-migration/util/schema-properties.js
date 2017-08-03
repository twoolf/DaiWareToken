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
const JsonCheckpoint = require('./checkpoint').JsonCheckpoint;
const logger = require('./logger');

const HIDDEN_FIELDS = [
  'stormpathMigrationRecoveryAnswer',
  'emailVerificationToken',
  'emailVerificationStatus'
];

function getSchemaProperty(key, type) {
  const property = {
    title: key,
    description: key,
    type,
    required: false
  };
  switch (type) {
  case 'array-string':
    property.type = 'array';
    property.items = { type: 'string' };
    property.union = 'DISABLE';
    break;
  case 'array-number':
    property.type = 'array';
    property.items = { type: 'number' };
    property.union = 'DISABLE';
    break;
  case 'boolean':
    break;
  case 'number':
    break;
  case 'string':
    property.minLength = 1;
    property.maxLength = 10000;
    break;
  default:
    throw new Error(`Unknown schema type: ${type}`);
  }

  if (key.indexOf('stormpathApiKey_') === 0 || HIDDEN_FIELDS.includes(key)) {
    property.permissions = [{
      principal: 'SELF',
      action: 'HIDE'
    }];
  } else {
    property.permissions = [{
      principal: 'SELF',
      action: 'READ_WRITE'
    }];
  }

  return property;
}

function compareKeys(key1, key2) {
  const apiKey = 'stormpathApiKey_';
  if (!key1.includes(apiKey) || !key2.includes(apiKey)) {
    return key1 > key2 ? 1 : (key1 === key2 ? 0 : -1);
  }
  return Number(key1.replace(apiKey, '')) - Number(key2.replace(apiKey, ''));
}

class SchemaProperties extends JsonCheckpoint {

  constructor() {
    super();
    this.properties = {};
  }

  checkpointConfig() {
    return {
      path: 'account-meta/schema',
      props: ['properties']
    };
  }

  add(key, type) {
    if (!this.properties[key]) {
      this.properties[key] = {};
    }
    if (!this.properties[key][type]) {
      this.properties[key][type] = 0;
    }
    this.properties[key][type]++;
  }

  /**
   * @returns {Object} { properties, schemaTypeMap }
   */
  getSchema() {
    const properties = {};
    const schemaTypeMap = {};

    Object.keys(this.properties).sort(compareKeys).forEach((key) => {
      const typeCountMap = this.properties[key];
      const types = Object.keys(typeCountMap);

      let pairs = [];
      let maxType;
      let maxCount = -1;
      for (let type of types) {
        const count = typeCountMap[type];
        pairs.push(`${type} (${count})`);
        if (count > maxCount) {
          maxCount = count;
          maxType = type;
        }
      }

      if (types.length > 1) {
        const msg = `Found multiple types for custom schema property '${key}' - ${pairs.join(' ')}.`;
        logger.warn(`${msg} Using the most common: ${maxType}.`);
      }

      schemaTypeMap[key] = maxType;
      properties[key] = getSchemaProperty(key, maxType);
    });

    // Additional properties needed for Stormpath integrations
    const tokenKey = 'emailVerificationToken';
    const tokenSchema = getSchemaProperty(tokenKey, 'string');
    tokenSchema.title = 'Email Verification Token';
    tokenSchema.description = 'Can be sent to the user to verify their email address';
    tokenSchema.maxLength = 64;
    schemaTypeMap[tokenKey] = 'string';
    properties[tokenKey] = tokenSchema;

    const statusKey = 'emailVerificationStatus';
    const statusSchema = getSchemaProperty(statusKey, 'string');
    statusSchema.title = 'Email Verification Status';
    statusSchema.description = 'Indicates if the user has verified their email address';
    statusSchema.maxLength = 32;
    schemaTypeMap[statusKey] = 'string';
    properties[statusKey] = statusSchema;

    const hrefKey = 'stormpathHref';
    const hrefSchema = getSchemaProperty(hrefKey, 'string');
    hrefSchema.title = 'Stormpath Account Href';
    hrefSchema.description = 'The href of this account from Stormpath';
    hrefSchema.maxLength = 128;
    schemaTypeMap[hrefKey] = 'string';
    properties[hrefKey] = hrefSchema;

    return { properties, schemaTypeMap };
  }

}

module.exports = SchemaProperties;
