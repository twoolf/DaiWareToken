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
const fs = require('fs');
const readdir = Promise.promisify(fs.readdir);
const path = require('path');
const { each, batch, mapToObject } = require('../util/concurrency');
const readFile = Promise.promisify(fs.readFile);
const config = require('../util/config');
const { warn } = require('../util/logger');

class FileIterator {

  constructor(dir, Klass, options, skipFiles = {}) {
    this.dir = dir;
    this.Klass = Klass;
    this.options = options;
    this.skipFiles = skipFiles;
  }

  async initialize() {
    try {
      const maxFiles = config.maxFiles || Infinity;
      let fileCount = 0;
      const files = await readdir(this.dir);

      this.files = files.filter((file) => {
        if (!file.endsWith('.json')) {
          return false;
        }
        fileCount++;
        if (this.skipFiles[path.basename(file, '.json')] || fileCount > maxFiles) {
          return false;
        }
        return true;
      });
    } catch (e) {
      warn(`Could not load ${this.dir}, skipping`);
      this.files = [];
    }
  }

  async readFile(file) {
    const filePath = `${this.dir}/${file}`;
    const contents = await readFile(filePath, 'utf8');
    const instance = new this.Klass(filePath);
    instance.setProperties(JSON.parse(contents));
    await instance.initializeFromExport(this.options);
    return instance;
  }

  each(fn, options) {
    const limit = options && options.limit || config.fileOpenLimit;
    return each(this.files, async (file) => {
      const instance = await this.readFile(file);
      return await fn(instance);
    }, limit);
  }

  batch(evalFn, batchFn, options) {
    const limit = options && options.limit || config.fileOpenLimit;
    return batch(
      this.files,
      async (file) => {
        const instance = await this.readFile(file);
        await evalFn(instance);
      },
      async (numProcessed) => {
        await batchFn(numProcessed);
      },
      limit
    );
  }

  mapToObject(fn, options) {
    const limit = options && options.limit || config.fileOpenLimit;
    return mapToObject(this.files, async (file, map) => {
      const instance = await this.readFile(file);
      return await fn(instance, map);
    }, limit);
  }

  get length() {
    return this.files.length;
  }

}

module.exports = FileIterator;
