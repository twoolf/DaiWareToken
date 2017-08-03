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

import org.apache.avro.generic.GenericRecord;

import kafka.consumer.ConsumerIterator;
import kafka.consumer.KafkaStream;
import kafka.message.MessageAndMetadata;

public class ConsumerLogic implements Runnable {
  private KafkaStream stream;
  private int threadNumber;

  public ConsumerLogic(KafkaStream stream, int threadNumber) {
    this.threadNumber = threadNumber;
    this.stream = stream;
  }

  public void run() {
    ConsumerIterator<Object, Object> it = stream.iterator();

    while (it.hasNext()) {
      MessageAndMetadata<Object, Object> record = it.next();

      String topic = record.topic();
      int partition = record.partition();
      long offset = record.offset();
      Object key = record.key();
      GenericRecord message = (GenericRecord) record.message();
      System.out.println("Thread " + threadNumber +
                         " received: " + "Topic " + topic +
                         " Partition " + partition +
                         " Offset " + offset +
                         " Key " + key +
                         " Message " + message.toString());
    }
    System.out.println("Shutting down Thread: " + threadNumber);
  }
}
