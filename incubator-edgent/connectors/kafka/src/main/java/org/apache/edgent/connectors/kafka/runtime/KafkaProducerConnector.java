/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

package org.apache.edgent.connectors.kafka.runtime;

import java.util.Map;

import org.apache.edgent.function.Supplier;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.common.serialization.ByteArraySerializer;

/**
 * A connector for producing/publishing Kafka key/value records.
 */
public class KafkaProducerConnector extends KafkaConnector implements AutoCloseable {
    private static final long serialVersionUID = 1L;
    private final Supplier<Map<String,Object>> configFn;
    private String id;
    private KafkaProducer<byte[],byte[]> producer;

    public KafkaProducerConnector(Supplier<Map<String, Object>> configFn) {
        this.configFn = configFn;
    }
    
    synchronized KafkaProducer<byte[],byte[]> client() {
        if (producer == null)
            producer = new KafkaProducer<byte[],byte[]>(configFn.get(),
                    new ByteArraySerializer(), new ByteArraySerializer());
        return producer;
    }

    @Override
    public synchronized void close() throws Exception {
        if (producer != null)
            producer.close();
    }
    
    String id() {
        if (id == null) {
            // include our short object Id
            id = "Kafka " + toString().substring(toString().indexOf('@') + 1);
        }
        return id;
    }
}
