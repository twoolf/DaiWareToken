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

import org.apache.edgent.execution.DirectSubmitter;
import org.apache.edgent.metrics.MetricsSetup;
import org.junit.Before;
import org.junit.Ignore;

@Ignore("abstract, provides common tests for concrete implementations")
public abstract class MetricsOnTest extends MetricsBaseTest {
    
    // Register Metrics service before each test.
    @Before
    public void createMetricRegistry() {
        metricRegistry = new WriteOnlyMetricRegistry();
        MetricsSetup.withRegistry(((DirectSubmitter<?,?>)getSubmitter()).getServices(), metricRegistry);
        // Don't start reporter thread as report is generated inside the test method
        // reporter.start(1, TimeUnit.SECONDS);
    }
}
