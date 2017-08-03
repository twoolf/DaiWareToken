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
const JsonCheckpoint = require('../util/checkpoint').JsonCheckpoint;
const logger = require('../util/logger');

class GroupMembershipMap extends JsonCheckpoint {

  constructor() {
    super();
    this.map = {};
  }

  checkpointConfig() {
    return {
      path: 'group-memberships/map',
      props: ['map']
    };
  }

  add(membership) {
    const groupId = membership.group.id;
    if (!this.map[groupId]) {
      this.map[groupId] = [];
    }
    this.map[groupId].push(membership.account.id);
  }

  processed() {
    return Object.keys(this.map).length > 0;
  }

  getMembershipMap() {
    return this.map;
  }

}

module.exports = GroupMembershipMap;
