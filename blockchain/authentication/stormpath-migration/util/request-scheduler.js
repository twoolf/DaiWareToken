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
'use strict';

const os = require('os');
const Promise = require('bluebird');
const rp = require('request-promise');
const logger = require('./logger');
const config = require('./config');
const ConcurrencyPool = require('./concurrency-pool');
const packageJson = require('../package.json');

// The max number of concurrent requests. Note, this is different from the
// concurrencyLimit in the config, which defines the max number of concurrent
// transactions (which can encompass more than one request).
// Note: Endpoints have different concurrency limits - for example,
// api/v1/apps/{id}/user/types/default?expand=schema has a limit of 100
const REQUEST_CONCURRENCY_LIMIT = 70;

/**
 * Calculates the time to schedule the next request in milliseconds - returns 0
 * if the rate limit hasn't been hit, otherwise the time to the next rate
 * limit reset.
 * @param {Object} headers
 * @param {String} headers['date']
 * @param {Number} headers['x-rate-limit-remaining']
 * @param {Number} headers['x-rate-limit-reset']
 * @returns {Number} number of milliseconds to next available request
 */
function timeToNextRequest(res) {
  const headers = res.headers || res.response.headers;
  const remaining = Number(headers['x-rate-limit-remaining']);

  // Must be greater than the concurrency limit because there could be
  // outstanding requests that have not finished. Add an extra 10 for buffer.
  if (remaining > REQUEST_CONCURRENCY_LIMIT + 10) {
    logger.silly(`x-rate-limit-remaining ${remaining}`);
    return 0;
  }

  const serverTimeUtcMs = Date.parse(headers.date);
  const serverResetUtcMs = headers['x-rate-limit-reset'] * 1000;

  // Add an extra buffer of 1000ms
  const time = serverResetUtcMs - serverTimeUtcMs + 1000;

  const rateLimit = headers['x-rate-limit-limit'];
  const msg = `Rate limit reached (${rateLimit}), scheduling next request in ${time}ms ${remaining}`;
  logger[remaining === 11 ? 'warn' : 'silly'](msg);

  return time;
}

/**
 * Schedules the next request, and executes it if concurrency and rate limits
 * are not hit.
 * @param {RequestScheduler} scheduler
 * @param {String} msg
 * @param {Function} fn
 */
async function schedule(scheduler, msg, fn) {
  const requestId = scheduler.requestId++;
  logger.silly(`Scheduling request id=${requestId}`, msg);
  const resource = await scheduler.pool.acquire();
  logger.silly(`Executing request id=${requestId}`);
  try {
    const res = await fn();
    logger.silly(`Finished request id=${requestId} status=SUCCESS`);
    setTimeout(resource.release, timeToNextRequest(res));
    return res.body;
  } catch (err) {
    logger.silly(`Finished request id=${requestId} status=FAILURE`);
    setTimeout(resource.release, timeToNextRequest(err));
    throw err;
  }
}

/**
 * Constructs user agent based on environment information
 */
function getUserAgent() {
  return `stormpath-migration/${packageJson.version} node/${process.versions.node} ${os.platform()}/${os.release()}`;
}

/**
 * Class that wraps request-promise with two enhancements:
 * 1. Limits the number of concurrent requests that are made at any given time
 * 2. Defers executing new requests if rate-limit is hit
 */
class RequestScheduler {

  /** Constructor */
  constructor() {
    this.requestId = 0;
    this.pool = new ConcurrencyPool(REQUEST_CONCURRENCY_LIMIT);
    this.rp = rp.defaults({
      baseUrl: config.oktaBaseUrl,
      headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json',
        'Authorization': `SSWS ${config.oktaApiToken}`,
        'User-Agent': getUserAgent()
      },
      resolveWithFullResponse: true,
      json: true,
      simple: true,
      agentOptions: {
          keepAlive: false
      }
    });
  }

  /** Wrapper around request-promise.get */
  get() {
    const msg = `GET ${JSON.stringify(arguments)}`;
    return schedule(this, msg, () => this.rp.get.apply(null, arguments));
  }

  /** Wrapper around request-promise.put */
  put() {
    const msg = `PUT ${JSON.stringify(arguments)}`;
    return schedule(this, msg, () => this.rp.put.apply(null, arguments));
  }

  /** Wrapper around request-promise.post */
  post() {
    const msg = `POST ${JSON.stringify(arguments)}`;
    return schedule(this, msg, () => this.rp.post.apply(null, arguments));
  }

  /** Wrapper around request-promise.delete */
  delete() {
    const msg = `DELETE ${JSON.stringify(arguments)}`;
    return schedule(this, msg, () => this.rp.delete.apply(null, arguments));
  }

}

module.exports = new RequestScheduler();
