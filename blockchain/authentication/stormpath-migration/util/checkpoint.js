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
const path = require('path');
const fs = require('fs-extra');
const config = require('./config');
const readFile = Promise.promisify(fs.readFile);
const writeFile = Promise.promisify(fs.writeFile);
const appendFile = Promise.promisify(fs.appendFile);
const { each } = require('./concurrency');
const { info } = require('./logger');

// Cache lookups for directory existence to reduce file i/o
const dirMap = new Map();
async function ensureDir(dir) {
  if (dirMap.get(dir)) {
    return;
  }
  await fs.ensureDir(dir);
  dirMap.set(dir, true);
}

class BaseCheckpoint {

  /**
   * @return {object} config - path, type, properties
   */
  checkpointConfig() {
    throw new Error('checkpointConfig must be implemented');
  }

  getCheckpointPathFromType(type) {
    return path.resolve(
      config.checkpointDir,
      `${this.checkpointConfig().path}.${type}`
    );
  }

  async readFile() {
    const filePath = this.getCheckpointPath();
    try {
      const content = await readFile(filePath, 'utf8');
      return this.parseFile(content);
    } catch (e) {
      return this.parseFile(null);
    }
  }

  async writeFile(content) {
    const filePath = this.getCheckpointPath();
    await ensureDir(path.dirname(filePath));
    await writeFile(filePath, content);
  }

  async appendFile(content) {
    const filePath = this.getCheckpointPath();
    await ensureDir(path.dirname(filePath));
    await appendFile(filePath, content);
  }

}

class JsonCheckpoint extends BaseCheckpoint {

  getCheckpointPath() {
    return this.getCheckpointPathFromType('json');
  }

  parseFile(content) {
    return content ? JSON.parse(content) : {};
  }

  setProperties(props) {
    Object.keys(props).forEach((key) => {
      this[key] = props[key];
    });
  }

  getProperties() {
    const props = {};
    this.checkpointConfig().props.forEach(key => props[key] = this[key]);
    return props;
  }

  async save() {
    const props = this.getProperties();
    await this.writeFile(JSON.stringify(props, null, 2));
  }

  async restore() {
    const props = await this.readFile();
    this.setProperties(props);
  }

}

class LogCheckpoint extends BaseCheckpoint {

  constructor(path) {
    super();
    this.path = path;
    this.pendingItems = [];
  }

  checkpointConfig() {
    return { path: this.path };
  }

  add(item) {
    this.pendingItems.push(item);
  }

  getCheckpointPath() {
    return this.getCheckpointPathFromType('txt');
  }

  parseFile(content) {
    return content ? content.split('\n') : [];
  }

  async save() {
    if (this.pendingItems.length === 0) {
      return;
    }
    await this.appendFile(this.pendingItems.join('\n') + '\n');
    this.pendingItems = [];
  }

  async process(processFn, limit) {
    info(`Loading log checkpoint file ${this.path}`);
    const items = await this.readFile();
    if (items.length === 0) {
      return;
    }

    info(`Processing ${items.length} log items`);
    await each(items, async (item) => {
      if (!item) {
        return;
      }
      await processFn(item);
    }, limit);
  }

}

module.exports = { JsonCheckpoint, LogCheckpoint }
