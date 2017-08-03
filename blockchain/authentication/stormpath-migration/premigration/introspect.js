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
const UnifiedAccounts = require('../stormpath/unified-accounts');
const GroupMembershipMap = require('../stormpath/group-membership-map');
const config = require('../util/config');
const stormpathExport = require('../stormpath/stormpath-export');
const cache = require('../migrators/util/cache');
const { batch } = require('../util/concurrency');

async function getDirectoryProviders() {
  const directories = await stormpathExport.getDirectories();
  logger.info(`Mapping ${directories.length} directories to providerIds`);
  return directories.mapToObject((directory, map) => {
    map[directory.id] = directory.provider.providerId;
  });
}

/**
 * Introspect the stormpath export and set up initial mappings:
 * 1. Custom schema definitions
 * 2. Unified accounts
 */
async function introspect() {
  logger.header('Introspecting stormpath export');
  const directoryProviders = await getDirectoryProviders();
  const accountLinks = await stormpathExport.getAccountLinks();
  const unifiedAccounts = new UnifiedAccounts(accountLinks);
  let numADLDAPAccounts = 0;
  let numUnverified = 0;

  const skipAccounts = await unifiedAccounts.restore();
  const accounts = await stormpathExport.getAccounts(skipAccounts);
  const totalAccounts = accounts.length;
  let nextProgressPoint = config.checkpointProgressLimit;

  logger.info(`Pre-processing ${accounts.length} stormpath accounts`);
  await accounts.batch(
    async (account) => {
      try {
        const providerId = directoryProviders[account.directory.id];
        if (!providerId) {
          unifiedAccounts.discardAccount(account);
          return logger.error(`Missing directory id=${account.directory.id}. Skipping account id=${account.id}.`);
        }
        if (providerId === 'ad' || providerId === 'ldap') {
          numADLDAPAccounts++;
          unifiedAccounts.discardAccount(account);
          return logger.verbose(`Skipping account id=${account.id}. Import using the Okta ${providerId.toUpperCase()} agent.`);
        }
        if (account.status === 'UNVERIFIED') {
          numUnverified++;
          unifiedAccounts.discardAccount(account);
          return logger.verbose(`Skipping unverified account id=${account.id}`);
        }

        await unifiedAccounts.addAccount(account);
      } catch (err) {
        logger.error(err);
      }
    },
    async (numProcessed) => {
      if (numProcessed >= nextProgressPoint || numProcessed === totalAccounts) {
        const percent = Math.round(numProcessed / totalAccounts * 100);
        logger.info(`-- Processed ${numProcessed} accounts (${percent}%) --`);
        nextProgressPoint += config.checkpointProgressLimit;
      }
      await unifiedAccounts.save();
    }
  );

  if (numADLDAPAccounts > 0) {
    logger.warn(`Skipped ${numADLDAPAccounts} AD or LDAP accounts. Import using the Okta AD or LDAP agent.`);
  }
  if (numUnverified > 0) {
    logger.warn(`Skipped ${numUnverified} unverified accounts.`);
  }

  const problemAccounts = unifiedAccounts.getProblemUsernameAccounts();
  if (problemAccounts.length > 0) {
    const lg = logger.group('Found stormpath usernames that conflict with other account usernames', 'error');
    for (let account of problemAccounts) {
      const username = account.username;
      const original = username.replace('@emailnotprovided.local', '');
      const sg = logger.group(`id=${account.id} orig_username=${original} new_username=${username}`, 'error');
      for (let conflict of account.conflicts) {
        logger.error(`Conflicts with id=${conflict.id} username=${conflict.username}`);
      }
      sg.end();
    }
    logger.error('Fix this by updating these usernames to emails, or contacting Okta support.');
    lg.end();
  }

  cache.unifiedAccounts = unifiedAccounts;

  const schema = unifiedAccounts.getSchema();
  cache.customSchemaProperties = schema.properties;
  cache.customSchemaTypeMap = schema.schemaTypeMap;

  logger.info(`Found ${Object.keys(cache.customSchemaProperties).length} custom schema properties`);

  const groupMembershipMap = new GroupMembershipMap();
  logger.info('Loading Stormpath groupMembershipMap from checkpoint file');
  await groupMembershipMap.restore();
  if (groupMembershipMap.processed()) {
    logger.info('Successfully loaded groupMembershipMap');
  } else {
    logger.info('No existing checkpoint file for groupMembershipMap');
    const groupMemberships = await stormpathExport.getGroupMemberships();
    const totalGroupMemberships = groupMemberships.length;
    let nextMembershipCheckpoint = config.checkpointProgressLimit;
    logger.info(`Pre-processing ${totalGroupMemberships} Stormpath group memberships`);
    await groupMemberships.batch(
      (membership) => groupMembershipMap.add(membership),
      (numProcessed) => {
        if (numProcessed >= nextMembershipCheckpoint || numProcessed === totalGroupMemberships) {
          const percent = Math.round(numProcessed / totalGroupMemberships * 100);
          logger.info(`-- Processed ${numProcessed} group memberships (${percent}%) --`);
          nextMembershipCheckpoint += config.checkpointProgressLimit;
        }
      }
    );
    logger.info('Saving groupMembershipMap to checkpoint file');
    await groupMembershipMap.save();
  }
  cache.groupMembershipMap = groupMembershipMap.getMembershipMap();
}

module.exports = introspect;
