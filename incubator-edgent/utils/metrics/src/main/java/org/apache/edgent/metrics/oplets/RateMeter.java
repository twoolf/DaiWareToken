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

import com.codahale.metrics.Meter;
import com.codahale.metrics.Metric;

/**
 * A metrics oplet which measures current tuple throughput and one-, five-, 
 * and fifteen-minute exponentially-weighted moving averages.
 * 
 * @param <T> Tuple type
 */
public final class RateMeter<T> extends SingleMetricAbstractOplet<T> {

    public static final String METRIC_NAME = "TupleRateMeter";
    private static final long serialVersionUID = 3328912985808062552L;
    private final Meter meter;

    public RateMeter() {
        super(METRIC_NAME);
        this.meter = new Meter();
    }

    @Override
    protected void peek(T tuple) {
        meter.mark();
    }

    @Override
    protected Metric getMetric() {
        return meter;
    }
}
