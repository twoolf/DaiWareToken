/**
 * Copyright 2015 Confluent Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package io.confluent.examples.consumer;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import kafka.api.FetchRequest;
import kafka.api.FetchRequestBuilder;
import kafka.api.PartitionOffsetRequestInfo;
import kafka.common.ErrorMapping;
import kafka.common.TopicAndPartition;
import kafka.javaapi.FetchResponse;
import kafka.javaapi.OffsetResponse;
import kafka.javaapi.PartitionMetadata;
import kafka.javaapi.TopicMetadata;
import kafka.javaapi.TopicMetadataRequest;
import kafka.javaapi.consumer.SimpleConsumer;
import kafka.message.MessageAndOffset;

public class SimpleConsumerExample {

  private List<String> replicaBrokers = new ArrayList<>();

  public SimpleConsumerExample() {
    replicaBrokers = new ArrayList<>();
  }

  // Find the Lead Broker for a Topic Partition
  private PartitionMetadata findLeader(List<String> seedBrokers, int port, String topic,
                                       int partition) {
    for (String seed : seedBrokers) {
      SimpleConsumer consumer = null;
      try {
        consumer = new SimpleConsumer(seed, port, 100000, 64 * 1024, "leaderLookup");
        List<String> topics = Collections.singletonList(topic);
        TopicMetadataRequest req = new TopicMetadataRequest(topics);
        kafka.javaapi.TopicMetadataResponse resp = consumer.send(req);

        List<TopicMetadata> metaData = resp.topicsMetadata();
        for (TopicMetadata item : metaData) {
          for (PartitionMetadata part : item.partitionsMetadata()) {
            if (part.partitionId() == partition) {
              replicaBrokers.clear();
              for (kafka.cluster.BrokerEndPoint replica : part.replicas()) {
                replicaBrokers.add(replica.host());
              }
              return part;
            }
          }
        }
      } catch (Exception e) {
        System.out.println("Error communicating with Broker [" + seed + "] to find Leader for ["
                           + topic + ", " + partition + "] Reason: " + e);
      } finally {
        if (consumer != null) consumer.close();
      }
    }
    return null;
  }

  // Determine Starting Offset
  public static long getLastOffset(SimpleConsumer consumer, String topic, int partition,
                                   long whichTime, String clientName) {
    TopicAndPartition topicAndPartition = new TopicAndPartition(topic, partition);
    Map<TopicAndPartition, PartitionOffsetRequestInfo> requestInfo =
        new HashMap<>();
    requestInfo.put(topicAndPartition, new PartitionOffsetRequestInfo(whichTime, 1));
    kafka.javaapi.OffsetRequest request = new kafka.javaapi.OffsetRequest(
        requestInfo, kafka.api.OffsetRequest.CurrentVersion(), clientName);
    OffsetResponse response = consumer.getOffsetsBefore(request);

    if (response.hasError()) {
      System.out.println("Error fetching data Offset Data the Broker. Reason: "
                         + response.errorCode(topic, partition) );
      return 0;
    }
    long[] offsets = response.offsets(topic, partition);
    return offsets[0];
  }

  private String findNewLeader(String oldLeader, String topic, int partition, int port)
      throws Exception {
    for (int i = 0; i < 3; i++) {
      PartitionMetadata metadata = findLeader(replicaBrokers, port, topic, partition);
      // first time through if the leader hasn't changed give ZooKeeper a second to recover
      // second time, assume the broker did recover before fail over, or it was a non-Broker issue
      if (!(metadata == null || metadata.leader() == null ||
            (oldLeader.equalsIgnoreCase(metadata.leader().host()) && i == 0))) {
        return metadata.leader().host();
      }
      try {
        Thread.sleep(1000);
      } catch (InterruptedException ie) {
        // ignore any exceptions
      }
    }
    throw new Exception("Unable to find new leader after Broker failure. Exiting");
  }

  public void run(long maxReads, String topic, int partition, List<String> seedBrokers,
                  int port) throws Exception {
    // find the metadata on the interested topic partition
    PartitionMetadata metadata = findLeader(seedBrokers, port, topic, partition);
    if (metadata == null) {
      System.out.println("Can't find metadata for Topic and Partition. Exiting");
      return;
    }
    if (metadata.leader() == null) {
      System.out.println("Can't find Leader for Topic and Partition. Exiting");
      return;
    }
    String leadBroker = metadata.leader().host();
    String clientName = "Client_" + topic + "_" + partition;

    SimpleConsumer consumer = new SimpleConsumer(leadBroker, port, 100000, 64 * 1024, clientName);
    long readOffset = getLastOffset(consumer,topic, partition,
                                    kafka.api.OffsetRequest.EarliestTime(), clientName);

    int numErrors = 0;
    while (maxReads > 0) {
      if (consumer == null) {
        consumer = new SimpleConsumer(leadBroker, port, 100000, 64 * 1024, clientName);
      }
      // Note: this fetchSize of 100000 might need to be increased if large batches are
      // written to Kafka
      int fetchSize = 100000;
      FetchRequest req = new FetchRequestBuilder()
          .clientId(clientName)
          .addFetch(topic, partition, readOffset, fetchSize)
          .build();
      FetchResponse fetchResponse = consumer.fetch(req);

      // Identify and Recover from Leader Changes
      if (fetchResponse.hasError()) {
        numErrors++;
        // Something went wrong!
        short code = fetchResponse.errorCode(topic, partition);
        System.out.println("Error fetching data from the Broker:" + leadBroker +
                           " Reason: " + code);
        if (numErrors > 5) break;
        if (code == ErrorMapping.OffsetOutOfRangeCode())  {
          // We asked for an invalid offset. For simple case ask for the last element to reset
          readOffset = getLastOffset(consumer,topic, partition,
                                     kafka.api.OffsetRequest.LatestTime(), clientName);
          continue;
        }
        consumer.close();
        consumer = null;
        leadBroker = findNewLeader(leadBroker, topic, partition, port);
        continue;
      }
      numErrors = 0;

      // Read the data
      long numRead = 0;
      for (MessageAndOffset messageAndOffset : fetchResponse.messageSet(topic, partition)) {
        long currentOffset = messageAndOffset.offset();
        if (currentOffset < readOffset) {
          System.out.println("Found an old offset: " + currentOffset + " Expecting: " + readOffset);
          continue;
        }
        readOffset = messageAndOffset.nextOffset();
        ByteBuffer payload = messageAndOffset.message().payload();

        byte[] bytes = new byte[payload.limit()];
        payload.get(bytes);
        System.out.println(String.valueOf(messageAndOffset.offset()) + ": "
                           + new String(bytes, "UTF-8"));
        numRead++;
        maxReads--;
      }

      if (numRead == 0) {
        try {
          Thread.sleep(1000);
        } catch (InterruptedException ie) {
          // ignore any exceptions
        }
      }
    }
    if (consumer != null) consumer.close();
  }

  public static void main(String args[]) {
    if (args.length != 5) {
      System.out.println("Please provide command line arguments: maxReads topic partitionId broker port");
      System.exit(-1);
    }

    long maxReads = Long.parseLong(args[0]); 
    String topic = args[1];
    int partition = Integer.parseInt(args[2]);
    List<String> seeds = new ArrayList<>();
    seeds.add(args[3]);
    int port = Integer.parseInt(args[4]);

    SimpleConsumerExample example = new SimpleConsumerExample();

    try {
      example.run(maxReads, topic, partition, seeds, port);
    } catch (Exception e) {
      System.out.println("Oops:" + e);
      e.printStackTrace();
    }
  }
}
