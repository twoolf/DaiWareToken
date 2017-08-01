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
package org.apache.edgent.providers.direct;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.atomic.AtomicLong;

import org.apache.edgent.topology.TStream;
import org.apache.edgent.topology.spi.tester.AbstractTester;
import org.apache.edgent.topology.tester.Condition;

class DirectTester extends AbstractTester {

    private final DirectTopology topology;

    DirectTester(DirectTopology topology) {
        this.topology = topology;
    }

    @Override
    public DirectTopology topology() {
        return topology;
    }

    @Override
    public Condition<Long> tupleCount(TStream<?> stream, final long expectedCount) {
        AtomicLong count = new AtomicLong();
        stream.sink(t -> {
            count.incrementAndGet();
        });
        return new Condition<Long>() {

            @Override
            public boolean valid() {
                return count.get() == expectedCount;
            }

            @Override
            public Long getResult() {
                return count.get();
            }
        };
    }

    @Override
    public <T> Condition<List<T>> streamContents(TStream<T> stream, @SuppressWarnings("unchecked") T... values) {
        List<T> contents = Collections.synchronizedList(new ArrayList<>());
        stream.sink(t -> contents.add(t));
        return new Condition<List<T>>() {

            @Override
            public boolean valid() {
                synchronized (contents) {
                    return Arrays.asList(values).equals(contents);
                }
            }

            @Override
            public List<T> getResult() {
                return contents;
            }
        };
    }

    @Override
    public Condition<Long> atLeastTupleCount(TStream<?> stream, long expectedCount) {
        AtomicLong count = new AtomicLong();
        stream.sink(t -> {
            count.incrementAndGet();
        });
        return new Condition<Long>() {

            @Override
            public boolean valid() {
                return count.get() >= expectedCount;
            }

            @Override
            public Long getResult() {
                return count.get();
            }
        };
    }

    @Override
    public <T> Condition<List<T>> contentsUnordered(TStream<T> stream, @SuppressWarnings("unchecked") T... values) {
        List<T> contents = Collections.synchronizedList(new ArrayList<>());
        stream.sink(t -> contents.add(t));
        return new Condition<List<T>>() {

            @Override
            public boolean valid() {
                synchronized (contents) {
                    if (contents.size() != values.length)
                        return false;
                    List<T> copy = new ArrayList<T>(contents);
                    for (T expected : values) {
                        if (!copy.remove(expected))
                            return false;
                    }
                    return copy.isEmpty();
                }
            }

            @Override
            public List<T> getResult() {
                return contents;
            }
        };
    }
}
