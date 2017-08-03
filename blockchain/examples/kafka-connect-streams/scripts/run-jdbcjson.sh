#!/usr/bin/env bash

# Copyright 2017 Confluent Inc.

TABLE="locations"
PACKAGE="jdbcjson"
TOPIC="$PACKAGE-$TABLE"

BOOTSTRAP_SERVERS=localhost:9092

sed -i "/^bootstrap.servers/c\bootstrap.servers=$BOOTSTRAP_SERVERS" files/$PACKAGE/connect-standalone.properties
scripts/helper/helper-connect.sh $TABLE $PACKAGE

mvn compile
TIMEOUT="20s"
echo -e "\n======= Running the application for $TIMEOUT ======="
timeout $TIMEOUT mvn exec:java -Dexec.mainClass=io.confluent.examples.connectandstreams.$PACKAGE.StreamsIngest -Dexec.args="$BOOTSTRAP_SERVERS"

scripts/helper/helper-show.sh $TOPIC
