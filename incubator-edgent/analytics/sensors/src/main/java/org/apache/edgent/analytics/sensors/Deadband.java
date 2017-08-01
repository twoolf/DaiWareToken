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
package org.apache.edgent.analytics.sensors;

import java.util.concurrent.TimeUnit;

import org.apache.edgent.function.Function;
import org.apache.edgent.function.Predicate;

/**
 * Deadband predicate function.
 *
 * @param <T> Tuple type.
 * @param <V> Value type for the deadband function.
 */
class Deadband<T, V> implements Predicate<T> {

    private static final long serialVersionUID = 1L;

    private final Function<T, V> valueFunction;
    private final Predicate<V> inBand;
    private final long period;
    private final TimeUnit unit;

    // Always send the first value.
    private transient boolean outOfBand = true;

    private transient long lastSend;
    
    Deadband(Function<T, V> valueFunction, Predicate<V> deadbandFunction) {
        this(valueFunction , deadbandFunction, 0, null);
    }

    Deadband(Function<T, V> valueFunction, Predicate<V> inBand, long period, TimeUnit unit) {
        this.valueFunction = valueFunction;
        this.inBand = inBand;
        this.period = period;
        this.unit = unit;
    }

    @Override
    public boolean test(final T t) {
        final V value = valueFunction.apply(t);
        boolean passTuple;
        long now = 0;
        if (!inBand.test(value)) {
            outOfBand = true;
            passTuple = true;
        } else if (outOfBand) {
            // When the value returns to being in-band
            // send the in-band value.
            outOfBand = false;
            passTuple = true;
        } else {
        	passTuple = false;
        	if (period != 0) {
                now = System.currentTimeMillis();
                long sinceLast = unit.convert(now - lastSend, TimeUnit.MILLISECONDS);
                if (sinceLast > period)
                     passTuple = true;
        	}
        }

        if (passTuple && period != 0)
            lastSend = now == 0 ? System.currentTimeMillis() : now;
            
        return passTuple;
    }

}

