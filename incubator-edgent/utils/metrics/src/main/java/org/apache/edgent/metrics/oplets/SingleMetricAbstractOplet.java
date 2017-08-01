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

import org.apache.edgent.oplet.OpletContext;
import org.apache.edgent.oplet.core.Peek;

import com.codahale.metrics.Metric;
import com.codahale.metrics.MetricRegistry;

/**
 * Base for metrics oplets which use a single metric object.
 * 
 * @param <T> Tuple type
 */
public abstract class SingleMetricAbstractOplet<T> extends Peek<T> {

    private static final long serialVersionUID = -6679532037136159885L;
    private final String shortMetricName;
    private String metricName;

    protected SingleMetricAbstractOplet(String name) {
        this.shortMetricName = name;
    }

    /**
     * Returns the name of the metric used by this oplet for registration.
     * The name uniquely identifies the metric in the {@link MetricRegistry}.
     * <p>
     * The name of the metric is {@code null} prior to oplet initialization,
     * or if this oplet has not been initialized with a 
     * {@code MetricRegistry}.
     * 
     * @return the name of the metric used by this oplet.
     */
    public String getMetricName() {
        return metricName;
    }

    protected abstract Metric getMetric();

    @Override
    public final void initialize(OpletContext<T, T> context) {
        super.initialize(context);

        MetricRegistry registry = context.getService(MetricRegistry.class);
        if (registry != null) {
            this.metricName = context.uniquify(shortMetricName);
            registry.register(getMetricName(), getMetric());
        }
    }

    @Override
    public final void close() throws Exception {
        MetricRegistry registry = getOpletContext().getService(MetricRegistry.class);
        if (registry != null) {
            registry.remove(getMetricName());
        }
    }
}
