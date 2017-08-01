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

package org.apache.edgent.connectors.mqtt.runtime;

import org.apache.edgent.function.Consumer;
import org.apache.edgent.function.Function;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.slf4j.Logger;

/**
 * Consumer that publishes stream tuples of type {@code T} to an MQTT server topic.
 *
 * @param <T> stream tuple type
 */
public class MqttPublisher<T> implements Consumer<T>, AutoCloseable {
    private static final long serialVersionUID = 1L;
    private final Logger logger;
    private transient String id;
    private final MqttConnector connector;
    private final Function<T, byte[]> payload;
    private final Function<T, String> topic;
    private final Function<T, Integer> qos;
    private final Function<T, Boolean> retain;

    public MqttPublisher(MqttConnector connector, Function<T, byte[]> payload, Function<T, String> topic,
            Function<T, Integer> qos, Function<T, Boolean> retain) {
        this.logger = connector.getLogger();
        this.connector = connector;
        this.payload = payload;
        this.topic = topic;
        this.qos = qos;
        this.retain = retain;
    }

    @Override
    public void accept(T t) {
        // right now, the caller of accept() doesn't do anything to
        // log or tolerate an unwind. address those issues here.
        String topicStr = topic.apply(t);
        try {
            MqttMessage message = new MqttMessage(payload.apply(t));
            message.setQos(qos.apply(t));
            message.setRetained(retain.apply(t));
            logger.trace("{} sending to topic:{}", id(), topicStr);
            connector.notIdle();
            connector.client().publish(topicStr, message);
        } catch (Exception e) {
            logger.error("{} sending to topic:{} failed.", id(), topicStr, e);
        }
    }
    
    protected String id() {
        if (id == null) {
            // use our short object Id
            id = connector.id() + " publisher " + toString().substring(toString().indexOf('@') + 1);
        }
        return id;
    }

    @Override
    public void close() throws Exception {
        connector.close();
    }
}
