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

package org.apache.edgent.connectors.pubsub;

import org.apache.edgent.connectors.pubsub.oplets.Publish;
import org.apache.edgent.connectors.pubsub.service.PublishSubscribeService;
import org.apache.edgent.execution.services.RuntimeServices;
import org.apache.edgent.function.Consumer;
import org.apache.edgent.function.Supplier;
import org.apache.edgent.topology.TSink;
import org.apache.edgent.topology.TStream;
import org.apache.edgent.topology.Topology;
import org.apache.edgent.topology.TopologyElement;

/**
 * Publish subscribe model.
 * <BR>
 * A stream can be {@link #publish(TStream, String, Class) published } to a topic
 * and then {@link #subscribe(TopologyElement, String, Class) subscribed} to by
 * other topologies.
 * 
 * <P>
 * A published topic has a type and subscribers must subscribe using the same
 * topic and type (inheritance matching is not supported).
 * <BR>
 * Multiple streams from different topologies can be published to
 * same topic (and type) and there can be multiple subscribers
 * from different topologies.
 * <BR>
 * A subscriber can exist before a publisher exists, they are connected
 * automatically when the job starts.
 * </P>
 * <P>
 * If no {@link PublishSubscribeService} is registered then published
 * tuples are discarded and subscribers see no tuples.
 * </P>
 * <P>
 * The recommended style for topics is MQTT topics, where {@code /}
 * is used to provide a hierarchy into topics. For example {@code engine/sensors/temperature}
 * might be a topic that represents temperature sensors in an engine.
 * <BR>
 * Topics that start with {@code edgent/} are reserved for use by Edgent.
 * <BR>
 * MQTT style wild-cards are not supported.
 * </P>
 */
public class PublishSubscribe {
    
    /**
     * Topics that start with {@value} are reserved for use by Edgent.
     */
    public static final String RESERVED_TOPIC_PREFIX= "edgent/";

    /**
     * Publish this stream to a topic.
     * This is a model that allows jobs to subscribe to 
     * streams published by other jobs.
     * 
     * @param <T> Tuple type
     * @param stream stream to publish
     * @param topic Topic to publish to.
     * @param streamType Type of objects on the stream.
     * @return sink element representing termination of this stream.
     * 
     * @see #subscribe(TopologyElement, String, Class)
     */
    public static <T> TSink<T> publish(TStream<T> stream, String topic, Class<? super T> streamType) {
        return stream.sink(new Publish<>(topic, streamType));
    }
        
    /**
     * Subscribe to a published topic.
     * This is a model that allows jobs to subscribe to 
     * streams published by other jobs.
     * @param <T> Tuple type
     * @param te TopologyElement whose Topology to add to
     * @param topic Topic to subscribe to.
     * @param streamType Type of the stream.
     * @return Stream containing published tuples.
     * 
     * @see #publish(TStream, String, Class)
     */
    public static <T> TStream<T> subscribe(TopologyElement te, String topic, Class<T> streamType) {
        
        Topology topology = te.topology();
        
        Supplier<RuntimeServices> rts = topology.getRuntimeServiceSupplier();
        
        return te.topology().events(new SubscriberSetup<T>(topic, streamType, rts));
    }
    
    /**
     * Subscriber setup function that adds a subscriber on
     * start up and removes it on close. 
     *
     * @param <T> Type of the tuples.
     */
    private static final class SubscriberSetup<T> implements Consumer<Consumer<T>>, AutoCloseable{
        private static final long serialVersionUID = 1L;
        
        private final Supplier<RuntimeServices> rts;
        private final String topic;
        private final Class<T> streamType;
        private Consumer<T> submitter;
        
        SubscriberSetup(String topic, Class<T> streamType, Supplier<RuntimeServices> rts) {
            this.topic = topic;
            this.streamType = streamType;
            this.rts = rts;
        }
        @Override
        public void accept(Consumer<T> submitter) {
            PublishSubscribeService pubSub = rts.get().getService(PublishSubscribeService.class);
            if (pubSub != null) {
                this.submitter = submitter;
                pubSub.addSubscriber(topic, streamType, submitter);
            }
        }
        @Override
        public void close() throws Exception {
            PublishSubscribeService pubSub = rts.get().getService(PublishSubscribeService.class);
            if (pubSub != null) {
                pubSub.removeSubscriber(topic, submitter);
            }
        }
    }
}
