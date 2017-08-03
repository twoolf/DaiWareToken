#!/usr/bin/env bash

# Copyright 2017 Confluent Inc.

TABLE="locations"
PACKAGE="javaproducer"
TOPIC="$PACKAGE-$TABLE"

BOOTSTRAP_SERVERS=localhost:9092
SCHEMA_REGISTRY_URL=http://localhost:8081
TABLE_LOCATIONS=/usr/local/lib/table.$TABLE

cp files/table.$TABLE $TABLE_LOCATIONS

mvn compile
timeout 10s mvn exec:java -Dexec.mainClass=io.confluent.examples.connectandstreams.$PACKAGE.Driver -Dexec.args="$BOOTSTRAP_SERVERS $SCHEMA_REGISTRY_URL $TABLE_LOCATIONS"
TIMEOUT="20s"
echo -e "\n======= Running the application for $TIMEOUT ======="
timeout $TIMEOUT mvn exec:java -Dexec.mainClass=io.confluent.examples.connectandstreams.$PACKAGE.StreamsIngest -Dexec.args="$BOOTSTRAP_SERVERS $SCHEMA_REGISTRY_URL"

scripts/helper/helper-show.sh $TOPIC

# View Schema Registry
echo -e "\n======= Viewing $TOPIC-value schema version 1 in Schema Registry ======="
curl -X GET $SCHEMA_REGISTRY_URL/subjects/$TOPIC-value/versions/1
