#!/usr/bin/env node

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

var matches = process.version.match(/v([0-9]+)\.([0-9]+)/);

var major = parseInt(matches[1]);

var minor = parseInt(matches[2]);

if ((major >= 7 && minor >= 6) || major >= 8) {
  require('./migrate');
} else {
  console.error('Node v7.6 or greater is required');
  process.exit(1);
}