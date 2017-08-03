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
const AccountRef = require('./account-ref');
const SchemaProperties = require('../util/schema-properties');
const LogCheckpoint = require('../util/checkpoint').LogCheckpoint;
const config = require('../util/config');
const logger = require('../util/logger');
const { each } = require('../util/concurrency');

function warn(account, msg) {
  logger.warn(`Account id=${account.id} email=${account.email} ${msg}`);
}

function getEmailPrefix(email) {
  return email.substring(0, email.indexOf('@'));
}

class UnifiedAccounts {

  constructor(accountLinks) {
    this.accountLinks = accountLinks;
    this.emailMap = {};
    this.stormpathAccountIdMap = {};
    this.loginPrefixAccountMap = {};
    this.convertedLoginAccounts = [];
    this.pendingAccountRefs = [];
    this.schemaProperties = new SchemaProperties();
    this.processedLog = new LogCheckpoint('account-meta/processed-accounts');
    this.discardLog = new LogCheckpoint('account-meta/discard-accounts');
    this.convertedLoginLog = new LogCheckpoint('account-meta/converted-logins');
  }

  discardAccount(account) {
    this.discardLog.add(account.id);
  }

  async save() {
    await each(
      this.pendingAccountRefs,
      ref => ref.save(),
      config.concurrencyLimit
    );
    await Promise.all([
      this.schemaProperties.save(),
      this.discardLog.save(),
      this.convertedLoginLog.save(),
      this.processedLog.save()
    ]);
  }

  async restore() {
    const skipAccounts = {};
    let numSkipAccounts = 0;

    await this.processedLog.process(async (accountId) => {
      const accountRef = new AccountRef(accountId);
      await accountRef.restore();
      this.setMaps(accountRef);
      skipAccounts[accountRef.id] = true;
      numSkipAccounts++;
      if (numSkipAccounts % config.checkpointProgressLimit === 0) {
        logger.info(`Loaded ${numSkipAccounts} processed accounts`);
      }
    }, config.fileOpenLimit);

    await this.discardLog.process((accountId) => {
      skipAccounts[accountId] = true;
      numSkipAccounts++;
    }, config.concurrencyLimit);

    if (numSkipAccounts > 0) {
      logger.info(`Found saved data for ${numSkipAccounts} processed accounts`);
    }

    await this.schemaProperties.restore();

    await this.convertedLoginLog.process((accountId) => {
      this.convertedLoginAccounts.push(this.stormpathAccountIdMap[accountId]);
    }, config.concurrencyLimit);

    return skipAccounts;
  }

  setMaps(accountRef) {
    this.emailMap[accountRef.email] = accountRef;
    this.stormpathAccountIdMap[accountRef.id] = accountRef;

    const loginPrefix = getEmailPrefix(accountRef.username);
    if (!this.loginPrefixAccountMap[loginPrefix]) {
      this.loginPrefixAccountMap[loginPrefix] = [];
    }
    this.loginPrefixAccountMap[loginPrefix].push(accountRef);
  }

  async addAccount(account) {
    const linkedAccountIds = this.accountLinks.getLinkedAccounts(account.id);

    // Verify account is not linked to a previously processed account with a
    // different email address
    const linkedAccounts = linkedAccountIds.map(id => this.stormpathAccountIdMap[id]);
    for (let linkedAccount of linkedAccounts) {
      if (linkedAccount && linkedAccount.email !== account.email) {
        warn(account, `is linked to id=${linkedAccount.id} email=${linkedAccount.email}, but email is different. Skipping.`);
        this.discardAccount(account);
        return;
      }
    }

    // Verify account does not have the same email as a previously processed
    // account that it is not linked to
    const emailAccountRef = this.emailMap[account.email];
    if (emailAccountRef && !linkedAccountIds.includes(emailAccountRef.id)) {
      warn(account, `has same email address as id=${emailAccountRef.id}, but is not linked. Skipping.`);
      this.discardAccount(account);
      return;
    }

    // If there is an existing account, merge it and return the merged account
    if (emailAccountRef) {
      const emailAccount = await emailAccountRef.mergeAccount(account);
      this.pendingAccountRefs.push(emailAccountRef);
      this.stormpathAccountIdMap[account.id] = emailAccountRef;
      this.addSchemaProperties(emailAccount);
      logger.info(`Merged account id=${account.id} email=${account.email} into linked account id=${emailAccountRef.id}`);
      this.discardAccount(account);
      return;
    }

    const accountRef = new AccountRef(account.id);
    accountRef.setProperties({
      email: account.email,
      username: account.username,
      accountFilePath: account.filePath
    });

    // By default, an Okta login must be formatted as an email address. If the
    // Stormpath username is not an email address, convert it by appending
    // @emailnotprovided.local.
    //
    // This will normally be okay for most cases - the email domain is not
    // necessary for a login lookup. However, for more complicated cases,
    // contact support to enable the REMOVE_EMAIL_FORMAT_LOGIN_RESTRICTION flag.
    if (!account.username.includes('@')) {
      const updated = `${account.username}@emailnotprovided.local`;
      logger.warn(`Account id=${account.id} username=${account.username} username is not an email. Using username=${updated}.`);
      account.username = updated;
      accountRef.setProperties({ username: updated });
      this.convertedLoginAccounts.push(accountRef);
      this.convertedLoginLog.add(accountRef.id);
    }

    logger.silly(`Adding new account id=${account.id}`);
    this.setMaps(accountRef);

    accountRef.setAccount(account);
    this.pendingAccountRefs.push(accountRef);

    this.addSchemaProperties(account);
    this.processedLog.add(accountRef.id);
  }

  getAccounts() {
    return Object.values(this.emailMap);
  }

  getUserIdByAccountId(accountId) {
    const accountRef = this.stormpathAccountIdMap[accountId];
    if (!accountRef) {
      return null;
    }
    return accountRef.oktaUserId;
  }

  getMissingAccounts(accountIds) {
    return accountIds.filter(accountId => !this.getUserIdByAccountId(accountId));
  }

  getUserIdsByAccountIds(accountIds) {
    const userIds = [];
    for (let accountId of accountIds) {
      const userId = this.getUserIdByAccountId(accountId);
      if (userId) {
        userIds.push(userId);
      }
    }
    return userIds;
  }

  /**
   * Problem usernames are defined as:
   * - Originally not an email address (@emailnotprovided.local appended)
   * - Have the same login prefix as another Stormpath username.
   *
   * For example, for two accounts:
   * - username1: susan -> susan@emailnotprovided.local
   * - username2: susan@example.com
   *
   * When logging in, susan@example.com will be used to logging in with the
   * previous 'susan@example.com' username. She is fine.
   *
   * However, susan@emailnotprovided.local will be used to logging in as
   * 'susan'. However, her new login is 'susan@emailnotprovided.local'.
   *
   * Note: If susan@example.com does not exist, susan@emailnotprovided.local
   * will be able to login with either 'susan' or 'susan@emailnotprovided.local'.
   * The domain is only necessary when there are multiple users with the same
   * login prefix.
   */
  getProblemUsernameAccounts() {
    const problems = [];
    for (const accountRef of this.convertedLoginAccounts) {
      const prefix = getEmailPrefix(accountRef.username);
      const prefixAccountRefs = this.loginPrefixAccountMap[prefix];
      const conflicts = prefixAccountRefs.filter((prefixAccountRef) => {
        return prefixAccountRef.id !== accountRef.id;
      });
      if (conflicts.length > 0) {
        problems.push({
          id: accountRef.id,
          username: accountRef.username,
          conflicts
        });
      }
    }
    return problems;
  }

  addSchemaProperties(account) {
    const customData = account.getCustomData();
    Object.keys(customData).forEach((key) => {
      this.schemaProperties.add(key, customData[key].type);
    });
  }

  getSchema() {
    return this.schemaProperties.getSchema();
  }

}

module.exports = UnifiedAccounts;
