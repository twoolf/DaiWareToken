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
package org.apache.edgent.runtime.etiao;

import org.apache.edgent.function.Consumer;
import org.apache.edgent.function.Functions;

/**
 * A forwarding Streamer whose destination
 * can be changed.
 * External synchronization or happens-before
 * guarantees must be provided by the object
 * owning an instance of {@code SettableForwarder}.
 *
 * @param <T> Type of data on the stream.
 */
public final class SettableForwarder<T> implements Consumer<T> {
    private static final long serialVersionUID = 1L;
    private Consumer<T> destination;

    /**
     * Create with the destination set to {@link Functions#discard()}.
     */
    public SettableForwarder() {
        this.destination = Functions.discard();
    }

    /**
     * Create with the specified destination.
     * @param destination Stream destination.
     */
    public SettableForwarder(Consumer<T> destination) {
        this.destination = destination;
    }
    
    @Override
    public void accept(T item) {
        getDestination().accept(item);
    }

    /**
     * Change the destination.
     * No synchronization is taken.
     * @param destination Stream destination.
     */
    public void setDestination(Consumer<T> destination) {
        this.destination = destination;
    }

    /**
     * Get the destination.
     * No synchronization is taken.
     * @return the destination
     */
    public final Consumer<T> getDestination() {
        return destination;
    }
}
