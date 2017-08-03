#!/usr/bin/env node

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
const v8 = require('v8');
const introspect = require('../premigration/introspect');
const logger = require('../util/logger');
const config = require('../util/config');

const migrateCustomSchema = require('../migrators/migrate-custom-schema');
const migrateAccounts = require('../migrators/migrate-accounts');
const migrateDirectories = require('../migrators/migrate-directories');
const migrateApplications = require('../migrators/migrate-applications');
const migrateGroups = require('../migrators/migrate-groups');
const migrateOrganizations = require('../migrators/migrate-organizations');

logger.setLevel(config.logLevel);
logger.info(`Starting import...`);
logger.info(`Writing log output to ${logger.logFile}`);

// Show v8 heap limit. Example increasing to 4Gb:
// node --max-old-space-size=4096 bin/migrate {{options}}
//
// Or, set it through the NODE_OPTIONS environment variable:
// export NODE_OPTIONS=--max-old-space-size=4096
const heapSize = Math.round(v8.getHeapStatistics().heap_size_limit / 1000000);
logger.info(`Heap size limit: ${heapSize}Mb`);

async function migrate() {
  try {
    const timer = logger.startTimer();

    await introspect();
    await migrateCustomSchema();
    await migrateAccounts();
    await migrateDirectories();
    await migrateGroups();
    await migrateOrganizations();
    await migrateApplications();

    logger.header('Done');
    timer.done(`Wrote log output to ${logger.logFile}`);
  } catch (err) {
    logger.error(err);
  }
}

migrate();
