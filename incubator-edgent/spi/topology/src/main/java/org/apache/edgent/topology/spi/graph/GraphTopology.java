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
package org.apache.edgent.topology.spi.graph;

import java.util.concurrent.TimeUnit;

import org.apache.edgent.function.Consumer;
import org.apache.edgent.function.Functions;
import org.apache.edgent.function.Supplier;
import org.apache.edgent.graph.Graph;
import org.apache.edgent.oplet.core.Source;
import org.apache.edgent.oplet.functional.Events;
import org.apache.edgent.oplet.functional.SupplierPeriodicSource;
import org.apache.edgent.oplet.functional.SupplierSource;
import org.apache.edgent.topology.TStream;
import org.apache.edgent.topology.plumbing.PlumbingStreams;
import org.apache.edgent.topology.spi.AbstractTopology;
import org.apache.edgent.topology.tester.Tester;

/**
 * Topology implementation that provides basic functions for implementing
 * source streams backed by a {@link Graph}.
 * 
 * @param <X> Tester type
 */
public abstract class GraphTopology<X extends Tester> extends AbstractTopology<X> {

    protected GraphTopology(String name) {
        super(name);
    }

    protected <N extends Source<T>, T> TStream<T> sourceStream(N sourceOp) {
        return new ConnectorStream<GraphTopology<X>, T>(this, graph().source(sourceOp));
    }

    @Override
    public <T> TStream<T> source(Supplier<Iterable<T>> data) {
        data = Functions.synchronizedSupplier(data);
        return sourceStream(new SupplierSource<>(data));
    }

    @Override
    public <T> TStream<T> poll(Supplier<T> data, long period, TimeUnit unit) {
        data = Functions.synchronizedSupplier(data);
        return sourceStream(new SupplierPeriodicSource<>(period, unit, data));
    }

    @Override
    public <T> TStream<T> events(Consumer<Consumer<T>> eventSetup) {
        TStream<T> rawEvents = sourceStream(new Events<>(eventSetup));
        return PlumbingStreams.isolate(rawEvents, true);
    }
}
