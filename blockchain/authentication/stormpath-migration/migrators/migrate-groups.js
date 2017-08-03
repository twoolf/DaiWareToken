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
const createOktaGroup = require('../functions/create-okta-group');
const addUsersToGroup = require('../functions/add-users-to-group');
const logger = require('../util/logger');
const stormpathExport = require('../stormpath/stormpath-export');
const config = require('../util/config');
const addUsersFromGroup = require('./util/add-users-from-group');
const cache = require('./util/cache');

async function migrateGroup(stormpathGroup) {
  const lg = logger.group(`Stormpath group id=${stormpathGroup.id} name=${stormpathGroup.name}`);
  try {
    const name = `group:${cache.directoryMap[stormpathGroup.directory.id]}:${stormpathGroup.name}`;
    let description = `Stormpath groupId=${stormpathGroup.id}`;
    if (stormpathGroup.description) {
      description += `: ${stormpathGroup.description}`;
    }
    const oktaGroup = await createOktaGroup(name, description);
    cache.groupMap[stormpathGroup.id] = oktaGroup.id;
    await addUsersFromGroup(stormpathGroup.id);
  } catch (err) {
    logger.error(err);
  } finally {
    lg.end();
  }
}

async function migrateGroups() {
  logger.header('Starting groups import');

  const stormpathGroups = await stormpathExport.getGroups();
  logger.info(`Importing ${stormpathGroups.length} groups`);

  return stormpathGroups.each(migrateGroup, { limit: 1 });
}

module.exports = migrateGroups;
