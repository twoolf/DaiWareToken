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
const Account = require('./account');
const JsonCheckpoint = require('../util/checkpoint').JsonCheckpoint;

class AccountRef extends JsonCheckpoint {

  constructor(id) {
    super();
    this.id = id;
  }

  checkpointConfig() {
    return {
      path: `account-refs/${this.id}`,
      props: ['id', 'oktaUserId', 'username', 'email', 'accountFilePath']
    };
  }

  // A pointer to the account is set during introspect, and is destroyed
  // when the accountRef is saved in a checkpoint. It's saved as a Promise
  // because merges can happen asynchronously - the Promise guarantees that
  // the account is only loaded once per checkpoint, and merges happen
  // sequentially.
  setAccount(account) {
    this.accountP = Promise.resolve(account);
  }

  // Note: The accountP pointer is *not* saved here because any lookup after
  // the introspect phase should not maintain the reference (eats up memory).
  async getAccount() {
    if (this.accountP) {
      return this.accountP;
    }
    const account = new Account(this.accountFilePath);
    await account.restore();
    return account;
  }

  async mergeAccount(accountToMerge) {
    if (!this.accountP) {
      this.accountP = this.getAccount();
    }
    const account = await this.accountP;
    account.merge(accountToMerge);
    return account;
  }

  async save() {
    await super.save();
    if (this.accountP) {
      const account = await this.accountP;
      await account.save();
      this.accountP = null;
    }
  }

}

module.exports = AccountRef;
