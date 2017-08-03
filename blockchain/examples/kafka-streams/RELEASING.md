# Release checklist

The following (perhaps not fully complete) checklist is helpful when performing a "release", which in this case means
a new branch for a specific Confluent release line (e.g. a `3.2.x` branch for the Confluent 3.2 release line).

- Create a release branch, if needed.  Example: `3.2.x` for the Confluent 3.2.x release line.
- Update `pom.xml`, notably `<version>`, `<kafka.version>`, `<confluent.version>`.
- For all instructions in e.g. `README.md` and Javadocs, remove `-SNAPSHOT` from the name of the packaged jar:

        # Snapshot = before release
        streams-examples-3.2.0-SNAPSHOT-standalone.jar

        # After release
        streams-examples-3.2.0-standalone.jar

- Update, if needed, any references in the instructions to "blobs" or links that are branch-based or tag-based.

        # Such links in the Javadocs would need updating (note the `3.2.x` token)
        <a href='https://github.com/confluentinc/examples/tree/3.2.x/kafka-streams#packaging-and-running'>Packaging</a>

  Here's an example command pipeline to update some such references from `3.1.x` to `3.2.x`:

    ```shell
    # Note: the `\1` prefix before `3.2.x` in the command below reference to
    # a so-called "capture group" in `sed` -- it is not a typo!
    $ find src -type f \
        | xargs grep "examples/tree/3.1.x/" \
        | cut -d ":" -f 1 \
        | xargs sed -i '' 's/\(\/examples\/tree\/\)3.1.x\//\13.2.x\//g'
    ```

- `README.md`: Update the version compatibility matrix by (1) adding a new entry for the new release and (2) updating
  the entry for the `master` branch.  Pay special attention to the version identifier of the Apache Kafka version.
- `README.md`: Update the Travis CI badge to point to the new release branch.

        # Example for 3.2.x
        [![Build Status](https://travis-ci.org/confluentinc/examples.svg?branch=3.2.x)](...etc...)


- Run sth like `git grep 3.2` (here: when releasing Confluent 3.2) to spot any references to the specific release
  version, and update the references where applicable.
- Run sth like `git grep 0.10.2.0` to spot any references to the specific Apache Kafka release version, and update the
  references where applicable.

As a follow-up step, you should also:

- Update the `master` branch to track the next line of development, typically based on snapshots/development versions.
