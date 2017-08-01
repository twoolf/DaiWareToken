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
package org.apache.edgent.topology.spi;

import java.util.Arrays;
import java.util.Collection;

import org.apache.edgent.function.Supplier;
import org.apache.edgent.topology.TStream;
import org.apache.edgent.topology.Topology;
import org.apache.edgent.topology.spi.functions.EndlessSupplier;
import org.apache.edgent.topology.tester.Tester;

/**
 * Topology implementation that uses the basic functions to implement most
 * sources streams.
 *
 * @param <X> Tester type
 */
public abstract class AbstractTopology<X extends Tester> implements Topology {

    private final String name;

    protected AbstractTopology(String name) {
        this.name = name;
    }

    @Override
    public Topology topology() {
        return this;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public TStream<String> strings(String... tuples) {
        return source(() -> Arrays.asList(tuples));
    }
    
    @SuppressWarnings("unchecked")
    @Override
    public <T> TStream<T> of(T... values) {
        return source(() -> Arrays.asList(values));
    }

    @Override
    public <T> TStream<T> generate(Supplier<T> data) {
        return source(new EndlessSupplier<T>(data));
    }

    X tester;

    @Override
    public X getTester() {
        if (tester == null)
            tester = newTester();
        return tester;
    }

    protected abstract X newTester();
    
    @Override
    public <T> TStream<T> collection(Collection<T> tuples) {
        return source(() -> tuples);
    }
}
