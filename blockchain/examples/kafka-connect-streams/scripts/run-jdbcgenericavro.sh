#!/usr/bin/env bash

# Copyright 2017 Confluent Inc.

TABLE="locations"
PACKAGE="jdbcgenericavro"
TOPIC="$PACKAGE-$TABLE"

BOOTSTRAP_SERVERS=localhost:9092
SCHEMA_REGISTRY_URL=http://localhost:8081

sed -i "/^bootstrap.servers/c\bootstrap.servers=$BOOTSTRAP_SERVERS" files/$PACKAGE/connect-standalone.properties
sed -i "/^value.converter.schema.registry.url/c\value.converter.schema.registry.url=$SCHEMA_REGISTRY_URL" files/$PACKAGE/connect-standalone.properties
scripts/helper/helper-connect.sh $TABLE $PACKAGE

mvn compile
TIMEOUT="20s"
echo -e "\n======= Running the application for $TIMEOUT ======="
timeout $TIMEOUT mvn exec:java -Dexec.mainClass=io.confluent.examples.connectandstreams.$PACKAGE.StreamsIngest -Dexec.args="$BOOTSTRAP_SERVERS $SCHEMA_REGISTRY_URL"

scripts/helper/helper-show.sh $TOPIC

# View Schema Registry
echo -e "\n======= Viewing $TOPIC-value schema version 1 in Schema Registry ======="
curl -X GET $SCHEMA_REGISTRY_URL/subjects/$TOPIC-value/versions/1
