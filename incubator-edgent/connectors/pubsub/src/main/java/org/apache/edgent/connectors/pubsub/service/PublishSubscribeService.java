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
package org.apache.edgent.connectors.pubsub.service;

import org.apache.edgent.function.Consumer;

/**
 * Publish subscribe service.
 * <BR>
 * Service that allows jobs to subscribe to 
 * streams published by other jobs.
 * <BR>
 * This is an optional service that allows streams
 * to be published by topic between jobs.
 * <P>
 * When an instance of this service is not available
 * then {@link org.apache.edgent.connectors.pubsub.PublishSubscribe#publish(org.apache.edgent.topology.TStream, String, Class) publish}
 * is a no-op, a sink that discards all tuples on the stream.
 * <BR>
 * A {@link org.apache.edgent.connectors.pubsub.PublishSubscribe#subscribe(org.apache.edgent.topology.TopologyElement, String, Class) subscribe} 
 * will have no tuples when an instance of this service is not available.
 * </P>
 * 
 * @see org.apache.edgent.connectors.pubsub.PublishSubscribe#publish(org.apache.edgent.topology.TStream, String, Class)
 * @see org.apache.edgent.connectors.pubsub.PublishSubscribe#subscribe(org.apache.edgent.topology.TopologyElement, String, Class)
 */
public interface PublishSubscribeService {
    
    /**
     * Add a subscriber to a published topic.
     * 
     * @param <T> Tuple type
     * @param topic Topic to subscribe to.
     * @param streamType Type of the stream.
     * @param subscriber How to deliver published tuples to the subscriber.
     */
    <T> void addSubscriber(String topic, Class<T> streamType, Consumer<T> subscriber);
    
    void removeSubscriber(String topic, Consumer<?> subscriber);
    
    /**
     * Get the destination for a publisher.
     * A publisher calls {@code destination.accept(tuple)} to publish
     * {@code tuple} to the topic.
     * 
     * @param <T> Tuple type
     * @param topic Topic tuples will be published to.
     * @param streamType Type of the stream
     * @return Consumer that is used to publish tuples.
     */
    <T> Consumer<T> getPublishDestination(String topic, Class<? super T> streamType);
}