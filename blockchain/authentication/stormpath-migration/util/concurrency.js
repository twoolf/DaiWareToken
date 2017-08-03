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

function cancelEach(list) {
  return () => {
    list.length = 0;
  };
}

async function processList(list, fn, cancelFn) {
  while (list.length > 0) {
    await fn(list.shift(), cancelFn);
  }
}

/**
 * Concurrently evaluates each item in a list (up to the limit) with the
 * provided function. This function can be async, but must return a Promise to
 * signal when it is complete.
 *
 * If an error should cancel any remaining operations, call the cancelFn to
 * clear out the remainder of the list. If this is not called, list processing
 * will continue processing in the background.
 *
 * @param {array} list
 * @param {function} fn
 * @param {number} limit
 */
async function each(list, fn, limit) {
  const promises = [];
  const cancelFn = cancelEach(list);
  for (let i = 0; i < limit; i++) {
    promises.push(processList(list, fn, cancelFn));
  }
  await Promise.all(promises);
}

/**
 * Evaluates a list with the provided function in batches of the limit. When
 * a batch is finished, the batchFn is called with the total number of
 * processed items. The evalFn and batchFn can both by async.
 *
 * @param {array} list
 * @param {function} evalFn
 * @param {function} batchFn
 * @param {number} limit
 */
async function batch(list, evalFn, batchFn, limit) {
  let numProcessed = 0;
  while (list.length > 0) {
    const promises = [];
    for (let i = 0; i < limit; i++) {
      if (list.length === 0) {
        break;
      }
      const item = list.shift();
      promises.push(evalFn(item));
      numProcessed++;
    }
    await Promise.all(promises);
    await batchFn(numProcessed);
  }
}

/**
 * Concurrently evaluates each item in a list (up to the limit) with the
 * provided mapping function, and returns the constructed map.
 *
 * @param {array} list
 * @param {function} fn
 * @param {number} limit
 */
async function mapToObject(list, fn, limit) {
  const map = {};
  await each(list, item => fn(item, map), limit);
  return map;
}

module.exports = { each, batch, mapToObject };
