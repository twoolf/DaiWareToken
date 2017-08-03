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

/**
 * Waits for all promises to be settled, and throws the first error if one of
 * the promises is rejected. Note, this is different from Promise.all because
 * it waits for all promises to be settled, including errors.
 * @param {*} promises
 * @returns {Promise}
 */
async function allSettled(promises) {
  const mapped = promises.map(promise => Promise.resolve(promise).reflect());
  const results = await Promise.all(mapped);
  const err = results.find(inspection => inspection.isRejected());
  if (err) {
    throw err.reason();
  }
  return results.map((inspection) => inspection.value());
}

module.exports = allSettled;
