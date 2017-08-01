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

import java.util.HashMap;
import java.util.Map;

import org.apache.edgent.function.Consumer;

/**
 * Publish subscribe service allowing exchange of streams between jobs in a provider.
 *
 */
public class ProviderPubSub implements PublishSubscribeService {
    
    private final Map<String,TopicHandler<?>> topicHandlers = new HashMap<>();
    
    @Override
    public <T> void addSubscriber(String topic, Class<T> streamType, Consumer<T> subscriber) { 
        getTopicHandler(topic, streamType).addSubscriber(subscriber);
    }
    
    @SuppressWarnings("unchecked")
    @Override
    public <T> Consumer<T> getPublishDestination(String topic, Class<? super T> streamType) {
        return (Consumer<T>) getTopicHandler(topic, streamType);      
    }
    
    @Override
    public void removeSubscriber(String topic, Consumer<?> subscriber) {
        TopicHandler<?> topicHandler;
        synchronized (this) {
            topicHandler = topicHandlers.get(topic);
        }
        if (topicHandler != null) {
            topicHandler.removeSubscriber(subscriber);
        }
    }
    
    @SuppressWarnings("unchecked")
    private synchronized <T> TopicHandler<T> getTopicHandler(String topic, Class<T> streamType) {
        TopicHandler<T> topicHandler = (TopicHandler<T>) topicHandlers.get(topic);

        if (topicHandler == null) {
            topicHandlers.put(topic, topicHandler = new TopicHandler<T>(streamType));
        } else {
            topicHandler.checkClass(streamType);
        }
        return topicHandler;
    } 
}
