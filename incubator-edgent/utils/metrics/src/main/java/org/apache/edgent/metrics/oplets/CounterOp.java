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
package org.apache.edgent.metrics.oplets;

import com.codahale.metrics.Counter;
import com.codahale.metrics.Metric;

/**
 * A metrics oplet which counts the number of tuples peeked at.
 * 
 * @param <T> Tuple type
 */
public final class CounterOp<T> extends SingleMetricAbstractOplet<T> {

    public static final String METRIC_NAME = "TupleCounter";
    private final Counter counter;
    private static final long serialVersionUID = -6679532037136159885L;

    public CounterOp() {
        super(METRIC_NAME);
        this.counter = new Counter();
    }

    @Override
    protected void peek(T tuple) {
        counter.inc();
    }

    @Override
    protected Metric getMetric() {
        return counter;
    }
}
