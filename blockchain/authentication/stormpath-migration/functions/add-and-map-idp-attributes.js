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

const MAPPINGS_PATH = `/api/internal/v1/mappings`;
const APPS_PATH = '/api/v1/apps';
const TYPES_PATH = `${APPS_PATH}/user/types`;
const SCHEMAS_PATH = '/api/v1/meta/schemas/user/default';

function getExternalToAppUserMap(schema) {
  const map = {};
  const properties = schema.schema.properties;
  for (let appUserAttribute of Object.keys(properties)) {
    map[properties[appUserAttribute].externalName] = appUserAttribute;
  }
  return map;
}

async function getSchemaInfo(idpId) {
  try {
    const types = await rs.get(`${APPS_PATH}/${idpId}/user/types/default?expand=schema`);
    const schemas = {};
    const externalToAppUserMap = {};

    // Existing base and custom schemas
    for (let schema of types._embedded.schemas) {
      schemas[schema.name] = schema;
    }

    // Imported schema for the IdP - this gives us any additional attributes
    // that have not already been mapped by default.
    if (types.type === 'IMPORTED') {
      schemas.imported = await rs.get(`${APPS_PATH}/${idpId}/user/imported/schema`);
      for (let schemaType of Object.keys(schemas)) {
        externalToAppUserMap[schemaType] = getExternalToAppUserMap(schemas[schemaType]);
      }
    } else {
      schemas.imported = {};
      externalToAppUserMap.custom = getExternalToAppUserMap(schemas.custom);
      externalToAppUserMap.base = externalToAppUserMap.imported = {};
    }

    return { typeId: types.id, type: types.type, schemas, externalToAppUserMap };
  } catch (err) {
    throw new ApiError(`Failed to get schema info for idpId=${idpId}`, err);
  }
}

async function addSamlAttributes(idpId, schemaInfo, mappings) {
  logger.verbose(`Adding attributes to saml idpId=${idpId} externalNames=[${mappings.map(m => m.externalName)}]`);
  try {
    const missing = [];
    const existing = [];
    const userSchema = await rs.get(SCHEMAS_PATH);
    const userProperties = Object.keys(userSchema.definitions).reduce((memo, type) => {
      return Object.assign(memo, userSchema.definitions[type].properties);
    }, {});

    for (let mapping of mappings) {
      if (schemaInfo.externalToAppUserMap.custom[mapping.externalName]) {
        existing.push(mapping);
      }
      else if (!userProperties[mapping.userAttribute]) {
        logger.warn(`Mapping found for unsupported user attribute ${mapping.userAttribute}, skipping`);
      }
      else {
        missing.push(mapping);
      }
    }

    if (existing.length > 0) {
      logger.exists(`IdP attributes: ${existing.map(m => m.externalName)}`);
    }

    if (missing.length === 0) {
      return;
    }

    logger.verbose(`Adding missing attributes to idpId=${idpId}: ${missing.map(m => m.externalName)}`);
    const customSchema = Object.assign({}, schemaInfo.schemas.custom);

    for (let mapping of missing) {
      const property = userProperties[mapping.userAttribute];
      property.externalName = mapping.externalName;
      customSchema.schema.properties[mapping.externalName] = property;
    }

    await rs.put({
      url: `${TYPES_PATH}/${schemaInfo.typeId}/schemas/${schemaInfo.schemas.custom.id}`,
      body: customSchema
    });
    logger.created(`IdP attributes: ${missing.map(m => m.externalName)}`);
  } catch (err) {
    throw new ApiError(`Failed to add attributes for idpId=${idpId}`, err);
  }
}

async function addSocialAttributes(idpId, schemaInfo, mappings) {
  logger.verbose(`Adding attributes to social idpId=${idpId} externalNames=[${mappings.map(m => m.externalName)}]`);

  const missing = [];
  const existing = [];
  const desiredExternal = mappings.map(mapping => mapping.externalName);

  for (let externalName of desiredExternal) {
    const inBase = schemaInfo.externalToAppUserMap.base[externalName];
    const inCustom = schemaInfo.externalToAppUserMap.custom[externalName];
    const inImported = schemaInfo.externalToAppUserMap.imported[externalName];

    if (inBase || inCustom) {
      existing.push(externalName);
    }
    else if (!inImported) {
      logger.warn(`Mapping found for unsupported external attribute ${externalName}, skipping`);
    }
    else {
      missing.push(externalName);
    }
  }

  if (existing.length > 0) {
    logger.exists(`IdP attributes: ${existing}`);
  }

  if (missing.length === 0) {
    return;
  }

  try {
    logger.verbose(`Adding missing attributes to idpId=${idpId}: ${missing}`);
    const customSchema = Object.assign({}, schemaInfo.schemas.custom);
    for (let externalName of missing) {
      const appUserAttribute = schemaInfo.externalToAppUserMap.imported[externalName];
      const property = schemaInfo.schemas.imported.schema.properties[appUserAttribute];
      customSchema.schema.properties[appUserAttribute] = property;
    }
    await rs.put({
      url: `${TYPES_PATH}/${schemaInfo.typeId}/schemas/${schemaInfo.schemas.custom.id}`,
      body: customSchema
    });
    logger.created(`IdP attributes: ${missing}`);
  } catch (err) {
    throw new ApiError(`Failed to add attributes for idpId=${idpId}`, err);
  }
}

async function mapIdpAttributes(idpId, mappings) {
  logger.verbose(`Mapping attributes for idpId=${idpId}`);

  // Reload schemaInfo because it can change after adding attributes
  const schemaInfo = await getSchemaInfo(idpId);

  const userTypes = await rs.get('/api/v1/user/types/default?expand=schema');
  const res = await rs.get({
    url: MAPPINGS_PATH,
    qs: {
      source: schemaInfo.typeId,
      target: userTypes.id
    }
  });
  const existingMappings = res[0];

  const desiredMappings = mappings.map((mapping) => {
    // If the target user profile attribute does not exist, warn and skip. This
    // occurs if the attribute does not exist on the base user profile and has
    // not been added as a custom user schema property.
    const validTarget = userTypes._embedded.schemas.find((schema) => {
      return schema.schema.properties[mapping.userAttribute];
    });
    if (!validTarget) {
      logger.warn(`Unknown user attribute for mapping '${mapping.userAttribute}', skipping`);
      return;
    }

    let appUserAttribute = schemaInfo.externalToAppUserMap.base[mapping.externalName];
    if (!appUserAttribute) {
      appUserAttribute = schemaInfo.externalToAppUserMap.custom[mapping.externalName];
    }
    if (!appUserAttribute) {
      appUserAttribute = schemaInfo.externalToAppUserMap.imported[mapping.externalName];
    }
    if (!appUserAttribute) {
      // If the external name does not exist on the AppUser, this means that
      // we were not able to add the attribute earlier. This occurs if, for
      // example, it hasn't been added to the known list of IdP mappings.
      logger.warn(`External name '${mapping.externalName}' is not available, skipping`);
      return;
    }

    return {
      sourceExpression: `appuser.${appUserAttribute}`,
      targetField: mapping.userAttribute,
      pushStatus: 'PUSH'
    };
  });

  const existing = [];
  const missing = [];

  for (let mapping of desiredMappings) {
    if (!mapping) {
      continue;
    }
    const exists = existingMappings.propertyMappings.find((current) => {
      const sameTarget = current.targetField === mapping.targetField;
      const sameSource = current.sourceExpression === mapping.sourceExpression;
      return sameTarget && sameSource;
    });
    if (exists) {
      existing.push(mapping);
    } else {
      missing.push(mapping);
    }
  }

  if (existing.length > 0) {
    logger.exists(`IdP attribute mappings`, existing);
  }

  if (missing.length === 0) {
    return;
  }

  try {
    for (let mapping of missing) {
      existingMappings.propertyMappings.push(mapping);
    }
    await rs.put({
      url: '/api/internal/v1/mappings',
      body: existingMappings
    });
    logger.created(`IdP attribute mappings`, missing);
  } catch (err) {
    throw new ApiError(`Failed to map attributes for idpId=${idpId}`, err);
  }
}

async function addAndMapIdpAttributes(idpId, mappings) {
  logger.verbose(`Adding and mapping attributes for idpId=${idpId}`);
  if (mappings.length === 0) {
    logger.verbose(`No mappings for idpId=${idpId}`);
    return;
  }

  const schemaInfo = await getSchemaInfo(idpId);
  if (schemaInfo.type === 'IMPORTED') {
    await addSocialAttributes(idpId, schemaInfo, mappings);
  } else {
    await addSamlAttributes(idpId, schemaInfo, mappings);
  }

  return mapIdpAttributes(idpId, mappings);
}

module.exports = addAndMapIdpAttributes;
