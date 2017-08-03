# stormpath-migration

## Requirements

- [Node JS 7.6 or higher](https://nodejs.org/en/download/)
- (Note to devs: set your IDE project language version to ECMAScript 6)

## Prerequisites
To use this tool, you must have a Stormpath export unzipped on your local filesystem. The directory structure should be as follows:
```
├── home/
│   ├── {tenantId}/
│   │   ├── directories/
│   │   │   ├── {directoryId}.json
│   │   │   ├-- ....
│   │   ├── providers/
│   │   │   ├── {directoryId}.json
│   │   │   ├-- ....
│   │   ├── accounts/
│   │   │   ├── {directoryId}/
│   │   │   │   ├── {accountId}.json
│   │   │   │   ├-- ....
│   │   │   ├-- ....
│   │   ├── groups/
│   │   │   ├── {directoryId}/
│   │   │   │   ├── {groupId}.json
│   │   │   │   ├-- ....
│   │   │   ├-- ....
│   │   ├── organizations/
│   │   │   ├── {organizationId}.json
│   │   │   ├-- ....
```
> Note: Providers must match 1:1 with Directories (same filenames).

> Note: The 'accounts' and 'groups' folders should be segmented by {directoryId} so that it's possible to iterate over them by directory.

Here's a concrete example:
```
├── home/
│   ├── tenant123/
│   │   ├── directories/
│   │   │   ├── 5LInED46hB6nv9auaOrIYW.json
│   │   │   ├── 7ZBZLdnlxFsEtIs4BRpUHk.json
│   │   │   ├-- ....
│   │   ├── providers/
│   │   │   ├── 5LInED46hB6nv9auaOrIYW.json
│   │   │   ├── 7ZBZLdnlxFsEtIs4BRpUHk.json
│   │   │   ├-- ....
│   │   ├── accounts/
│   │   │   ├── 5LInED46hB6nv9auaOrIYW/
│   │   │   │   ├── 8LJuP3l2Lke9XWL4Vpie3o.json
│   │   │   │   ├-- ....
│   │   │   ├── 7ZBZLdnlxFsEtIs4BRpUHk/
│   │   │   │   ├── 4DfxGCAyrxNyiqjPQIHfHI.json
│   │   │   │   ├-- ....
│   │   ├── groups/
│   │   │   ├── 5LInED46hB6nv9auaOrIYW/
│   │   │   │   ├── 1iMYLWrjvnc833sPCBVbtU.json
│   │   │   │   ├-- ....
│   │   │   ├── 7ZBZLdnlxFsEtIs4BRpUHk/
│   │   │   │   ├── d72ghS4bBhaqzuUN6ur1g.json
│   │   │   │   ├-- ....
│   │   ├── organizations/
│   │   │   ├── 7O67Ni1CG5bo9E9NLA3kdg.json
│   │   │   ├-- ....
```

> In this example, the "stormpathBaseDir" would be `/home/tenant123`.

### To Install:
```
$ npm install -g @okta/stormpath-migration
```

### To Run:
```
$ import-stormpath --stormPathBaseDir /path/to/export/data --oktaBaseUrl https://your-org.okta.com --oktaApiToken 5DSfsl4x@3Slt6
```

*Note*: output is logged to the console as well as to a json log file. The first and last line of output
indicate where the JSON log file was written to.

### Required Args

#### `--stormPathBaseDir (-b)`

Root directory where your Stormpath tenant export data lives

- Example: `--stormPathBaseDir ~/Desktop/stormpath-exports/683IDSZVtUQewtFoqVrIEe`

#### `--oktaBaseUrl (-u)`

Base URL of your Okta tenant

- Example: `--oktaBaseUrl https://your-org.okta.com`

#### `--oktaApiToken (-t)`

API token for your Okta tenant (SSWS token)

- Example: `--oktaApiToken 00gdoRRz2HUBdy06kTDwTOiPeVInGKpKfG-H4P_Lij`

### Optional Args

#### `--customData (-d)`

Strategy for importing Stormpath Account custom data. Defaults to `flatten`.

- Options:

  - `flatten` - Add [custom user profile schema properties](http://developer.okta.com/docs/api/resources/schemas.html#user-profile-schema-property-object) for each custom data property. Use this for simple custom data objects.
  - `stringify` - Stringify the Account custom data object into one `customData` [custom user profile schema property](http://developer.okta.com/docs/api/resources/schemas.html#user-profile-schema-property-object). Use this for more complex custom data objects.
  - `exclude` - Exclude Stormpath Account custom data from the import

- Example: `--customData stringify`

#### `--concurrencyLimit (-c)`

Max number of concurrent transactions. Defaults to `30`.

- Example: `--concurrencyLimit 200`

#### `--maxFiles (-f)`

Max number of files to parse per directory. You can use this to preview a large import. For example, if you set the argument `--maxFiles 50`, the import tool will only run for the first 50 accounts, groups, directories, and other Stormpath objects that are imported.

- Example: `--maxFiles 50`

#### `--checkpointDir`

When the import script starts, it tries to map the Stormpath data model to the Okta model - for example, finding all unique custom schema attributes in the Account objects, or mapping linked Accounts to the same Okta user. For large exports, this can take a long time and sometime cause CPU or memory issues.

Incremental import state is saved to the `--checkpointDir`, which defaults to `{cwd}/tmp`. If the import script is stopped, it will load this incremental state on the next run.

**Note**, when running from a saved state, warnings and messages from previous runs will not be written to the console. Output from previous runs is stored in the `logs` directory.

- Example: `--checkpointDir /Users/me/tmp`

#### `--checkpointProgressLimit`

The number of accounts to process before logging a progress message to the console. This value is only used during the initial phase of the import, when the import tool introspects the Stormpath export data.

If you are working with a large export, you may want to increase this limit to reduce the number of messages that are logged to the console. The default value is `10000`.

- Example: `--checkpointProgressLimit 50000`

#### `--fileOpenLimit`

Max number of files to read concurrently from the Stormpath export directory. For large exports, you may want to raise this limit to increase thoroughput in processing the Stormpath export files.

**Note**, raising this limit will increase the CPU and memory usage of the import tool. Defaults to `100`.

- Example: `--fileOpenLimit 1000`

#### `--logLevel (-l)`

Logging level. Defaults to `info`.

- Options: `error`, `warn`, `info`, `verbose`, `debug`, `silly`
- Example: `--logLevel verbose`

### Node memory errors

If you're working with a large export and get a `process out of memory` error, you can increase the limit by setting the `NODE_OPTIONS` environment variable:

```bash
# --max-old-space-size is in Megabytes. In this example, the limit is 4Gb.
$ export NODE_OPTIONS=--max-old-space-size=4096
```

After starting the import, this memory limit is logged to the console:

```bash
[2017-07-23 18:50:29] INFO     Starting import...
[2017-07-23 18:50:29] INFO     Writing log output to {{log_location}}
[2017-07-23 18:50:29] INFO     Heap size limit: 4329Mb
```

### Organization Reset

If you need to run the import script again, but wish to start with a blank slate, this tool also provides a reset script that will remove all data from your org. The reset script takes the same arguments as the import script.

**WARNING: This will delete all data from the specified org:**

```bash
reset-okta --stormPathBaseDir /path/to/export/data --oktaBaseUrl https://your-org.okta.com --oktaApiToken 5DSfsl4x@3Slt6
```
