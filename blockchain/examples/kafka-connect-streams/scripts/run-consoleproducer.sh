#!/usr/bin/env bash

# Copyright 2017 Confluent Inc.

TABLE="locations"
PACKAGE="consoleproducer"
TOPIC="$PACKAGE-$TABLE"

BOOTSTRAP_SERVERS=localhost:9092

TABLE_LOCATIONS=/usr/local/lib/table.$TABLE
cp files/table.$TABLE $TABLE_LOCATIONS

# View contents of file
echo -e "\n======= Contents of $TABLE_LOCATIONS ======="
cat $TABLE_LOCATIONS

# Write the contents of the file TABLE_LOCATIONS to the Topic `consoleproducer`, where the id is the message key and the name and sale are the message value.
echo -e "\n======= Running kafka-console-producer ======="
cat $TABLE_LOCATIONS | \
kafka-console-producer \
--broker-list $BOOTSTRAP_SERVERS \
--topic $TOPIC \
--property parse.key=true \
--property key.separator='|'

mvn compile
TIMEOUT="20s"
echo -e "\n======= Running the application for $TIMEOUT ======="
timeout $TIMEOUT mvn exec:java -Dexec.mainClass=io.confluent.examples.connectandstreams.$PACKAGE.StreamsIngest -Dexec.args="$BOOTSTRAP_SERVERS"

scripts/helper/helper-show.sh $TOPIC
