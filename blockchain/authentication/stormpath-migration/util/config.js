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
const yargs = require('yargs');
const fs = require('fs');
const path = require('path');

const usage = `
Migration tool to import Stormpath data into an Okta tenant

Usage:
  node $0 \\
     --stormPathBaseDir {/path/to/export/data} \\
     --oktaBaseUrl {https://your-org.okta.com} \\
     --oktaApiToken {token}
`;

const config = yargs
  .usage(usage)
  .options({
    stormPathBaseDir: {
      description: 'Root directory where Stormpath export data lives',
      required: true,
      alias: 'b'
    },
    oktaBaseUrl: {
      description: 'Base URL of your Okta tenant',
      required: true,
      alias: 'u'
    },
    oktaApiToken: {
      description: 'API token for your Okta tenant (SSWS token)',
      required: true,
      alias: 't'
    },
    customData: {
      description: 'Strategy for importing Account custom data',
      required: false,
      alias: 'd',
      choices: ['flatten', 'stringify', 'exclude'],
      default: 'flatten'
    },
    concurrencyLimit: {
      description: 'Max number of concurrent transactions',
      required: false,
      alias: 'c',
      default: 30
    },
    maxFiles: {
      description: 'Max number of files to parse per directory. Use to preview the entire import.',
      required: false,
      alias: 'f'
    },
    fileOpenLimit: {
      description: 'Max number of files to read at any given time.',
      required: false,
      default: 100
    },
    checkpointDir: {
      description: 'Directory to save checkpoint files to',
      required: false,
      default: path.resolve(process.cwd(), 'tmp')
    },
    checkpointProgressLimit: {
      description: 'Number of accounts to process before logging progress message',
      required: false,
      default: 10000
    },
    logLevel: {
      description: 'Logging level',
      required: false,
      alias: 'l',
      choices: ['error', 'warn', 'info', 'verbose', 'debug', 'silly'],
      default: 'info'
    }
  })
  .example('\t$0 --stormPathBaseDir /path/to/export/data --oktaBaseUrl https://your-org.okta.com --oktaApiToken 5DSfsl4x@3Slt6', '')
  .check(function(argv, aliases) {
      if (!fs.existsSync(argv.stormPathBaseDir)) {
        return `'${argv.stormPathBaseDir} is not a valid stormpath base directory'`;
      }
      return true;
  })
  .version()
  .argv;

if (config.stormPathBaseDir.endsWith('/')) {
  config.stormPathBaseDir = config.stormPathBaseDir.slice(0, -1);
}

config.isCustomDataFlatten = config.customData === 'flatten';
config.isCustomDataStringify = config.customData === 'stringify';
config.isCustomDataExclude = config.customData === 'exclude';

module.exports = config;
