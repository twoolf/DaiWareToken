package io.confluent.examples.producer;

import JavaSessionize.avro.LogLine;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.Producer;
import org.apache.kafka.clients.producer.ProducerRecord;

import java.util.Properties;
import java.util.Random;

public class AvroClicksProducer {

    public static void main(String[] args) throws InterruptedException {
        if (args.length != 1) {
            System.out.println("Please provide command line arguments: schemaRegistryUrl");
            System.exit(-1);
        }

        String schemaUrl = args[0];

        Properties props = new Properties();
        // hardcoding the Kafka server URI for this example
        props.put("bootstrap.servers", "localhost:9092");
        props.put("acks", "all");
        props.put("retries", 0);
        props.put("key.serializer", "io.confluent.kafka.serializers.KafkaAvroSerializer");
        props.put("value.serializer", "io.confluent.kafka.serializers.KafkaAvroSerializer");
        props.put("schema.registry.url", schemaUrl);

        // Hard coding topic too.
        String topic = "clicks";

        // Hard coding wait between events so demo experience will be uniformly nice
        int wait = 500;

        Producer<String, LogLine> producer = new KafkaProducer<String, LogLine>(props);

        // We keep producing new events and waiting between them until someone ctrl-c
        while (true) {
            LogLine event = EventGenerator.getNext();
            System.out.println("Generated event " + event.toString());

            // Using IP as key, so events from same IP will go to same partition
            ProducerRecord<String, LogLine> record = new ProducerRecord<String, LogLine>(topic, event.getIp().toString(), event);
            producer.send(record);
            Thread.sleep(wait);
        }
    }
}

