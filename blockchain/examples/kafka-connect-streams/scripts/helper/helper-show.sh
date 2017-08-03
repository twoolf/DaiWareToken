#!/usr/bin/env bash

TOPIC=$1
BOOTSTRAP_SERVERS=localhost:9092
ZOOKEEPER=localhost:2181

# Run the Consumer to print the key as well as the value from the Topic
echo -e "\n======= Running kafka-console-consumer for topic $TOPIC ======="
timeout 5s kafka-console-consumer \
--bootstrap-server $BOOTSTRAP_SERVERS \
--from-beginning \
--topic $TOPIC \
--property print.key=true

# View all topics
echo -e "\n======= Viewing all topics in Kafka ======="
kafka-topics --zookeeper $ZOOKEEPER --list
