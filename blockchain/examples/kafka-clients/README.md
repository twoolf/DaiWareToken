# Kafka client examples

This directory includes projects demonstrating how to use the Java Kafka producer
and consumer. You can find detailed explanation of the code at the
[application development section](http://docs.confluent.io/3.3.0/app-development.html)
of the Confluent Platform documentation.


# Requirements

* The examples described in this README require the version of Kafka shipped with Confluent Platform 2.0.1.


# Quickstart

Before running the examples, we must launch Zookeeper, Kafka, and Schema Registry.
In what follows, we assume that Zookeeper, Kafka, and Schema Registry are started with the default settings.
See the [Confluent Quickstart guide](http://docs.confluent.io/3.3.0/quickstart.html) for detailed instructions.

```shell
# Start Zookeeper. Since this is a long-running service, you should run it in its own terminal.
$ ./bin/zookeeper-server-start ./etc/kafka/zookeeper.properties

# Start Kafka, also in its own terminal.
$ ./bin/kafka-server-start ./etc/kafka/server.properties

# Start the Schema Registry, also in its own terminal.
./bin/schema-registry-start ./etc/schema-registry/schema-registry.properties
```

Then create a topic called `page_visits`:

```shell
# Create page_visits topic
$ ./bin/kafka-topics --create --zookeeper localhost:2181 --replication-factor 1 \
                   --partitions 1 --topic page_visits
```

At this point Zookeeper, Kafka, and Schema Registry are up and running.

Now we can turn our attention to the client examples in this directory.

First run the example producer in the [producer](producer) sub-folder to publish 10 data records to Kafka.

```shell
$ cd producer
# Build the producer app
$ mvn clean package
# Run the producer
$ mvn exec:java -Dexec.mainClass="io.confluent.examples.producer.ProducerExample" \
  -Dexec.args="10 http://localhost:8081"
```

Then run the Kafka consumer application in the [consumer](consumer) sub-folder to read the records we just published
to the Kafka cluster, and to display the records in the console.

```shell
$ cd ../consumer
# Build the consumer app
$ mvn clean package
# Run the consumer
$ mvn exec:java -Dexec.mainClass="io.confluent.examples.consumer.ConsumerGroupExample" \
  -Dexec.args="localhost:2181 group page_visits 1 http://localhost:8081"
```
