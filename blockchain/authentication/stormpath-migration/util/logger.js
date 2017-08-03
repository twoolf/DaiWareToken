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
const winston = require('winston');
const chalk = require('chalk');
const path = require('path');
const fs = require('fs');
const prettyJson = require('prettyjson');
const columnify = require('columnify');
const ApiError = require('./api-error');

function prependSpaces(str) {
  return str.replace(/\n/g, '\n  ');
}

function pad(number) {
  return number < 10 ? `0${number}` : number;
}

function formatTime(time) {
  const date = new Date(time);
  return date.getFullYear() +
    '-' + pad(date.getMonth() + 1) +
    '-' + pad(date.getDate()) +
    ' ' + pad(date.getHours()) +
    ':' + pad(date.getMinutes()) +
    ':' + pad(date.getSeconds());
}

function formatHeader(header) {
  return `\n${chalk.underline(header.toUpperCase())}:`;
}

function formatLine(title, options) {
  const date = chalk.dim(`[${formatTime(options.timestamp())}]`);
  let message = options.message || '';

  // API Errors
  if (options.meta && options.meta.name === 'ApiError') {
    message = options.meta.message;
    options.meta = {};
  }

  // Unhandled API Errors - fail gracefully
  else if (options.meta && options.meta.error) {
    const error = new ApiError('Failed to make request', options.meta);
    message = error.message;
    options.meta = {};
  }

  // No meta to output, just return the message
  const metaKeys = options.meta ? Object.keys(options.meta) : [];
  if (metaKeys.length === 0) {
    return `${date} ${title}${message}`;
  }

  let metaStr = '';
  // An "array" - winston converts this to an object. For arrays, output
  // in columns (i.e. custom schema properties)
  if (options.meta['0']) {
    const arr = metaKeys.map(key => options.meta[key]);
    metaStr = columnify(arr, {
      columnSplitter: '  '
    });
  }

  // Other objects that are logged
  else {
    metaStr = prettyJson.render(options.meta);
  }

  return `${date} ${title}${message}\n${prependSpaces(metaStr)}`;
}

// Create logger directory if it does not exist, and generate a unique name
// for this current run.
const logDir = path.resolve(__dirname, '../logs');
if (!fs.existsSync(logDir)) {
  fs.mkdirSync(logDir);
}
const time = (new Date().toISOString()).replace(/:|\./g, '');
const logFile = `${logDir}/import-${time}.json`;

let indent = 0;

const levels = {
  error: {
    title: chalk.red.bold('ERROR    ')
  },
  warn: {
    title: chalk.bgBlack.yellow.bold('WARN     ')
  },
  exists: {
    title: chalk.bold('EXISTS   ')
  },
  updated: {
    title: chalk.green.bold('UPDATED  ')
  },
  created: {
    title: chalk.green.bold('CREATED  ')
  },
  from: {
    title: chalk.bold('FROM     '),
  },
  header: {
    title: chalk.bold('START    ')
  },
  info: {
    title: chalk.bold('INFO     ')
  },
  verbose: {
    title: chalk.cyan.bold('VERBOSE  ')
  },
  debug: {
    title: chalk.magenta.bold('DEBUG    ')
  },
  silly: {
    title: chalk.green.bold('SILLY    ')
  }
};

const levelMap = {};
Object.keys(levels).forEach((key, i) => levelMap[key] = i);

const logger = new (winston.Logger)({
  exitOnError: false,
  levels: levelMap,
  transports: [
    new (winston.transports.Console)({
      level: 'info',
      timestamp: () => Date.now(),
      formatter: (options) => {
        // Header is a special line that breaks up the log output
        const level = options.level;
        if (level === 'header') {
          return formatHeader(options.message);
        }

        let title = levels[level].title;
        for (i = 0; i < indent; i++) {
          title += '  ';
        }
        if (indent > 0) {
          title += '└─ ';
        }
        return formatLine(title, options);
      }
    }),
    new (winston.transports.File)({
      filename: logFile
    })
  ]
});

logger.setLevel = (level) => {
  logger.transports.console.level = level;
};

logger.group = (title, type) => {
  if (!type) {
    type = 'from';
  }
  logger[type](title);
  indent++;
  return {
    end: () => indent--
  };
}

logger.logFile = logFile;

module.exports = logger;
