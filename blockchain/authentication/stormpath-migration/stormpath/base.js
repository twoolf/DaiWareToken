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
const config = require('../util/config');

class Base extends JsonCheckpoint {

  constructor(filePath) {
    super();
    this.filePath = filePath;
  }

  checkpointConfig() {
    return {
      path: this.filePath.replace(`${config.stormPathBaseDir}/`, '').replace('.json', ''),
      props: Object.keys(this)
    };
  }

  /**
   * Override this to perform initialization options that only happen when
   * loading from the export. Any properties that are created will be saved
   * to the checkpoint file.
   */
  initializeFromExport() {}

}

module.exports = Base;
