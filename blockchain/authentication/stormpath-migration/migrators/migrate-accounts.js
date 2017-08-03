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
const createOktaUser = require('../functions/create-okta-user');
const logger = require('../util/logger');
const config = require('../util/config');
const { each } = require('../util/concurrency');
const cache = require('./util/cache');

async function migrateAccounts() {
  logger.header(`Starting users import`);
  const accountRefs = cache.unifiedAccounts.getAccounts();
  logger.info(`Importing ${accountRefs.length} unified Stormpath accounts`);
  try {
    await each(accountRefs, async (accountRef, cancel) => {
      try {
        const account = await accountRef.getAccount();
        const user = await createOktaUser(
          account.getProfileAttributes(),
          account.getCredentials(),
          account.getStatus()
        );
        accountRef.setProperties({ oktaUserId: user.id });
        await accountRef.save();
        cache.userIdAccountMap[user.id] = accountRef;
        for (let directoryId of account.directoryIds) {
          if (!cache.directoryUserMap[directoryId]) {
            cache.directoryUserMap[directoryId] = [];
          }
          cache.directoryUserMap[directoryId].push(user.id);
        }
      } catch (err) {
        logger.error(err);
        if (err.message.includes('Maximum number of users has been reached')) {
          cancel();
          throw new Error('Reached maximum number of users - contact support to raise this limit');
        }
      }
    }, config.concurrencyLimit);
  } catch (err) {
    logger.error(`Failed to import all accounts: ${err.message}`);
  }
}

module.exports = migrateAccounts;
