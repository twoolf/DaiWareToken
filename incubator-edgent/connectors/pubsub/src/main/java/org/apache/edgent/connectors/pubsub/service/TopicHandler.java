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

import java.util.HashSet;
import java.util.Set;

import org.apache.edgent.function.Consumer;

class TopicHandler<T> implements Consumer<T> {
    private static final long serialVersionUID = 1L;

    private final Class<T> streamType;
    private final Set<Consumer<T>> subscribers = new HashSet<>();

    TopicHandler(Class<T> streamType) {
        this.streamType = streamType;
    }

    synchronized void addSubscriber(Consumer<T> subscriber) {
        subscribers.add(subscriber);
    }

    synchronized void removeSubscriber(Consumer<?> subscriber) {
        subscribers.remove(subscriber);
    }

    @Override
    public synchronized void accept(T tuple) {
        for (Consumer<T> subscriber : subscribers)
            subscriber.accept(tuple);
    }

    void checkClass(Class<T> streamType) {
        if (this.streamType != streamType)
            throw new IllegalArgumentException();
    }
}
