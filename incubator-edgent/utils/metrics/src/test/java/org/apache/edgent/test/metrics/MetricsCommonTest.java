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
package org.apache.edgent.test.metrics;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.edgent.function.Consumer;
import org.apache.edgent.metrics.oplets.CounterOp;
import org.apache.edgent.metrics.oplets.RateMeter;
import org.apache.edgent.oplet.JobContext;
import org.apache.edgent.oplet.Oplet;
import org.apache.edgent.oplet.OpletContext;
import org.apache.edgent.oplet.OutputPortContext;
import org.apache.edgent.oplet.core.AbstractOplet;
import org.apache.edgent.oplet.core.Peek;
import org.junit.Test;

import com.codahale.metrics.MetricRegistry;

public class MetricsCommonTest {
    @Test
    public void counterOpHierachy() {
        assertTrue(Oplet.class.isAssignableFrom(CounterOp.class));
        assertTrue(AbstractOplet.class.isAssignableFrom(CounterOp.class));
        assertTrue(Peek.class.isAssignableFrom(CounterOp.class));
    }
    
    @Test
    public void rateMeterHierachy() {
        assertTrue(Oplet.class.isAssignableFrom(RateMeter.class));
        assertTrue(AbstractOplet.class.isAssignableFrom(RateMeter.class));
        assertTrue(Peek.class.isAssignableFrom(RateMeter.class));
    }
    
    @Test
    public void metricNameRateMeter() throws Exception {
        Context<Object,Object> ctx = new Context<>();
        ctx.addService(MetricRegistry.class, new MetricRegistry());
        
        RateMeter<Object> op = new RateMeter<>();
        op.initialize(ctx);
        assertNotNull(op.getMetricName());
        op.close();
    }

    @Test
    public void metricNullNameRateMeter() throws Exception {
        Context<Object,Object> ctx = new Context<>();
        RateMeter<Object> op = new RateMeter<>();
        
        op.initialize(ctx);
        assertNull(op.getMetricName());
        op.close();
    }

    @Test
    public void metricNameCounter() throws Exception {
        Context<Object,Object> ctx = new Context<>();
        ctx.addService(MetricRegistry.class, new MetricRegistry());
        
        CounterOp<Object> op = new CounterOp<>();
        op.initialize(ctx);
        assertNotNull(op.getMetricName());
        op.close();
    }

    @Test
    public void metricNullNameCounter() throws Exception {
        Context<Object,Object> ctx = new Context<>();
        CounterOp<Object> op = new CounterOp<>();
        
        op.initialize(ctx);
        assertNull(op.getMetricName());
        op.close();
    }

    private static class Context<I, O> implements OpletContext<I, O> {
        private final Map<Class<?>, Object> services = new HashMap<>();

        public <T> T addService(Class<T> serviceClass, T service) {
            return serviceClass.cast(services.put(serviceClass, service));
        }

        @Override
        public String getId() {
            throw new UnsupportedOperationException();
        }

        @Override
        public <T> T getService(Class<T> serviceClass) {
            return serviceClass.cast(services.get(serviceClass));
        }

        @Override
        public int getInputCount() {
            throw new UnsupportedOperationException();
        }

        @Override
        public int getOutputCount() {
            throw new UnsupportedOperationException();
        }

        @SuppressWarnings("serial")
        @Override
        public List<? extends Consumer<O>> getOutputs() {
            List<Consumer<O>> outputs = new ArrayList<>();
            outputs.add(0, new Consumer<O>() {
                @Override
                public void accept(O value) {}
            });
            return outputs;
        }

        @Override
        public JobContext getJobContext() {
            throw new UnsupportedOperationException();
        }

        @Override
        public String uniquify(String name) {
            return "unique." + name;
        }

        @Override
        public List<OutputPortContext> getOutputContext() {
            throw new UnsupportedOperationException();
        }
    }
}
