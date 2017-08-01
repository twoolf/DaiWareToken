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
package org.apache.edgent.connectors.file;

import org.apache.edgent.function.Predicate;

/**
 * FileWriter active file flush configuration control.
 * <p>
 * Flushing of the active file can be any combination of:
 * <ul>
 * <li>after every {@code cntTuple} tuples written</li>
 * <li>after {@code tuplePredicate} returns true</li>
 * <li>after {@code periodMsec} has elapsed since the last time based flush</li>
 * </ul>
 * If nothing specific is specified, the underlying buffered
 * writer's automatic flushing is utilized.
 * 
 * @param <T> stream tuple type
 */
public class FileWriterFlushConfig<T> {
    private int cntTuples;
    private long periodMsec;
    private Predicate<T> tuplePredicate;
    
    /**
     * Create a new configuration.
     * <p>
     * The underlying buffered writer's automatic flushing is used.
     * <p>
     * Same as {@code newConfig(0, 0, null)}
     * 
     * @param <T> Tuple type
     * @return the flush configuration
     */
    public static <T> FileWriterFlushConfig<T> newImplicitConfig() {
        return newConfig(0,0,null);
    }
    /** same as {@code newConfig(cntTuples, 0, null)}
     * 
     * @param <T> Tuple type
     * @param cntTuples flush every {@code cntTuple} tuples written. 0 to disable.
     * @return the flush configuration
     */
    public static <T> FileWriterFlushConfig<T> newCountBasedConfig(int cntTuples) {
        if (cntTuples < 1)
            throw new IllegalArgumentException("cntTuples");
        return newConfig(cntTuples, 0, null);
    }
    /** same as {@code newConfig(0, periodMsec, null)}
     * 
     * @param <T> Tuple type
     * @param periodMsec flush every {@code periodMsec} milliseconds.  0 to disable.
     * @return the flush configuration
     */
    public static <T> FileWriterFlushConfig<T> newTimeBasedConfig(long periodMsec) {
        if (periodMsec < 1)
            throw new IllegalArgumentException("periodMsec");
        return newConfig(0, periodMsec, null);
    }
    /** same as {@code newConfig(0, 0, tuplePredicate)}
     * 
     * @param <T> Tuple type
     * @param tuplePredicate flush if {@code tuplePredicate} is true. null to disable.
     * @return the flush configuration
     */
    public static <T> FileWriterFlushConfig<T> newPredicateBasedConfig(Predicate<T> tuplePredicate) {
        if (tuplePredicate == null)
            throw new IllegalArgumentException("tuplePredicate");
        return newConfig(0, 0, tuplePredicate);
    }
    /**
     * Create a new configuration.
     * <p>
     * If nothing specific is specified, the underlying buffered
     * writer's automatic flushing is utilized.
     *
     * @param <T> Tuple type
     * @param cntTuples flush every {@code cntTuple} tuples written. 0 to disable.
     * @param periodMsec flush every {@code periodMsec} milliseconds.  0 to disable.
     * @param tuplePredicate flush if {@code tuplePredicate} is true. null to disable.
     * @return the flush configuration
     */
    public static <T> FileWriterFlushConfig<T> newConfig(int cntTuples, long periodMsec, Predicate<T> tuplePredicate) {
        return new FileWriterFlushConfig<>(cntTuples, periodMsec, tuplePredicate);
    }
    
    private FileWriterFlushConfig(int cntTuples, long periodMsec, Predicate<T> tuplePredicate) {
        if (cntTuples < 0)
            throw new IllegalArgumentException("cntTuples");
        if (periodMsec < 0)
            throw new IllegalArgumentException("periodMsec");
        this.cntTuples = cntTuples;
        this.periodMsec = periodMsec;
        this.tuplePredicate = tuplePredicate;
    }    
    
    /**
     * Get the tuple count configuration value.
     * @return the value
     */
    public int getCntTuples() { return cntTuples; }
    
    /**
     * Get the time period configuration value.
     * @return the value
     */
    public long getPeriodMsec() { return periodMsec; }
    
    /**
     * Get the tuple predicate configuration value.
     * @return the value
     */
    public Predicate<T> getTuplePredicate() { return tuplePredicate; }
    
    /**
     * Evaluate if the specified values indicate that a flush should be
     * performed.
     * @param nTuples number of tuples written to the active file
     * @param tuple the tuple written to the file
     * @return true if a flush should be performed.
     */
    public boolean evaluate(int nTuples, T tuple) {
        return (cntTuples > 0 && nTuples > 0 && nTuples % cntTuples == 0)
                || (tuplePredicate != null && tuplePredicate.test(tuple));
    }
    
    @Override
    public String toString() {
        return String.format("cntTuples:%d periodMsec:%d tuplePredicate:%s",
                getCntTuples(), getPeriodMsec(),
                getTuplePredicate() == null ? "no" : "yes");
    }

}
