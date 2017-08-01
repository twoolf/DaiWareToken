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
package org.apache.edgent.analytics.math3.json;

import com.google.gson.JsonElement;
import com.google.gson.JsonObject;

/**
 * Univariate aggregator for JSON tuples.
 * This is the runtime implementation interface
 * of {@link JsonUnivariateAggregate} defined aggregate.
 */
public interface JsonUnivariateAggregator {
    
    /**
     * Clear the aggregator to prepare for a new aggregation.
     * @param partitionKey Partition key.
     * @param n Number of tuples to be aggregated.
     */
    void clear(JsonElement partitionKey, int n);
    
    /**
     * Add a value to the aggregation. 
     * @param value Value to be added.
     */
    void increment(double value);
    
    /**
     * Place the result of the aggregation into the {@code result}
     * object. The key for the result must be
     * {@link JsonUnivariateAggregate#name()} for the corresponding
     * aggregate. The value of the aggregation may be a primitive value
     * such as a {@code double} or any valid JSON element.
     * 
     * @param partitionKey Partition key.
     * @param result JSON object holding the result.
     */
    void result(JsonElement partitionKey, JsonObject result);
}
