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

function releaseFrom(pool) {
  return function () {
    if (pool.pending.length === 0) {
      pool.numActive--;
      return;
    }
    pool.pending.shift().resolve();
  };
}

class ConcurrencyPool {

  constructor(maxConcurrent) {
    this.numActive = 0;
    this.pending = [];
    this.maxConcurrent = maxConcurrent;
  }

  acquire() {
    const resource = { release: releaseFrom(this) };

    if (this.numActive < this.maxConcurrent) {
      this.numActive++;
      return Promise.resolve(resource);
    }

    let resolve, reject;
    const promise = new Promise((innerResolve, innerReject) => {
      resolve = innerResolve;
      reject = innerReject;
    });
    this.pending.push({
      resolve: () => resolve(resource),
      reject
    });
    return promise;
  }

}

module.exports = ConcurrencyPool;
