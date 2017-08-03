This is a small example showing how to produce specific Avro classes to Kafka
Using Confluent's Schema Repository and Avro Serializers.

Specific Avro classes mean that we use Avro's code generation to generate the LogLine class, then populate it and produce to Kafka.
There is also an example using generic Avro, without code generation: https://github.com/confluentinc/examples

To build this producer:

    $ cd specific-avro-producer
    $ mvn clean package
    
Quickstart
-----------

Before running the examples, make sure that Zookeeper, Kafka and Schema Registry are
running. In what follows, we assume that Zookeeper, Kafka and Schema Registry are
started with the default settings.

    # Start Zookeeper
    $ bin/zookeeper-server-start config/zookeeper.properties

    # Start Kafka
    $ bin/kafka-server-start config/server.properties

    # Start Schema Registry
    $ bin/schema-registry-start config/schema-registry.properties
    
If you don't already have a schema registry, you will need to install it.
Either from packages: http://www.confluent.io/developer
Or from source: https://github.com/confluentinc/schema-registry
    
Then create a topic called clicks:

    # Create page_visits topic
    $ bin/kafka-topics.sh --create --zookeeper localhost:2181 --replication-factor 1 \
      --partitions 1 --topic clicks
      

Then run the producer to produce two clicks every second:

    $ java -cp target/uber-clickstream-generating-producer-1.0-SNAPSHOT.jar io.confluent.examples.producer.AvroClicksProducer http://localhost:8081
    
You can validate the result by using the avro console consumer (part of the schema repository):

    $ bin/kafka-avro-console-consumer --zookeeper localhost:2181 --topic clicks --from-beginning