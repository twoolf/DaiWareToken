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
 * FileWriter active file cycle configuration control.
 * <p>
 * Cycling the active file closes it, gets it to its final pathname,
 * and induces the application of a retention policy
 * {@link FileWriterRetentionConfig}.
 * <p>
 * Cycling the active file can be any combination of:
 * <ul>
 * <li>after {@code fileSize} bytes have been written</li>
 * <li>after every {@code cntTuple} tuples written</li>
 * <li>after {@code tuplePredicate} returns true</li>
 * <li>after {@code periodMsec} has elapsed since the last time based cycle</li>
 * </ul>
 * 
 * @param <T> stream tuple type
 */
public class FileWriterCycleConfig<T> {
    private long fileSize;
    private int cntTuples;
    private long periodMsec;
    private Predicate<T> tuplePredicate;
    
    /** same as {@code newConfig(fileSize, 0, 0, null)}
     * 
     * @param <T> Tuple type
     * @param fileSize cycle after {@code fileSize} bytes have been written. 0 to disable.
     * @return the cycle configuration
     */
    public static <T> FileWriterCycleConfig<T> newFileSizeBasedConfig(long fileSize) {
        if (fileSize < 1)
            throw new IllegalArgumentException("fileSize");
        return newConfig(fileSize, 0, 0, null);
    }
    /** same as {@code newConfig(0, cntTuples, 0, null)}
     * 
     * @param <T> Tuple type
     * @param cntTuples cycle after every {@code cntTuple} tuples have been written. 0 to disable.
     * @return the cycle configuration
     */
    public static <T> FileWriterCycleConfig<T> newCountBasedConfig(int cntTuples) {
        if (cntTuples < 1)
            throw new IllegalArgumentException("cntTuples");
        return newConfig(0, cntTuples, 0, null);
    }
    /** same as {@code newConfig(0, 0, periodMsec, null)}
     * 
     * @param <T> Tuple type
     * @param periodMsec cycle after {@code periodMsec} has elapsed since the last time based cycle. 0 to disable.
     * @return the cycle configuration
     */
    public static <T> FileWriterCycleConfig<T> newTimeBasedConfig(long periodMsec) {
        if (periodMsec < 1)
            throw new IllegalArgumentException("periodMsec");
        return newConfig(0, 0, periodMsec, null);
    }
    /** same as {@code newConfig(0, 0, 0, tuplePredicate)}
     * 
     * @param <T> Tuple type
     * @param tuplePredicate cycle if {@code tuplePredicate} returns true. null to disable.
     * @return the cycle configuration
     */
    public static <T> FileWriterCycleConfig<T> newPredicateBasedConfig(Predicate<T> tuplePredicate) {
        return newConfig(0, 0, 0, tuplePredicate);
    }
    
    /**
     * Create a new configuration.
     * <p>
     * At least one configuration mode must be enabled.
     * @param <T> Tuple type
     * @param fileSize cycle after {@code fileSize} bytes have been written. 0 to disable.
     * @param cntTuples cycle after every {@code cntTuple} tuples have been written. 0 to disable.
     * @param periodMsec cycle after {@code periodMsec} has elapsed since the last time based cycle. 0 to disable.
     * @param tuplePredicate cycle if {@code tuplePredicate} returns true. null to disable.
     * @return the cycle configuration
     */
    public static <T> FileWriterCycleConfig<T> newConfig(long fileSize, int cntTuples, long periodMsec, Predicate<T> tuplePredicate) {
        return new FileWriterCycleConfig<>(fileSize, cntTuples, periodMsec, tuplePredicate);
    }

    private FileWriterCycleConfig(long fileSize, int cntTuples, long periodMsec, Predicate<T> tuplePredicate) {
        if (fileSize < 0)
            throw new IllegalArgumentException("fileSize");
        if (cntTuples < 0)
            throw new IllegalArgumentException("cntTuples");
        if (periodMsec < 0)
            throw new IllegalArgumentException("periodMsec");
        if (fileSize==0 && cntTuples==0 && periodMsec==0 && tuplePredicate==null)
            throw new IllegalArgumentException("no cycle configuration specified");
            
        this.fileSize = fileSize;
        this.cntTuples = cntTuples;
        this.periodMsec = periodMsec;
        this.tuplePredicate = tuplePredicate;
    }
    
    /**
     * Get the file size configuration value.
     * @return the value
     */
    public long getFileSize() { return fileSize; }
    
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
     * Evaluate if the specified values indicate that a cycling of
     * the active file should be performed.
     * @param fileSize the number of bytes written to the active file
     * @param nTuples number of tuples written to the active file
     * @param tuple the tuple written to the file
     * @return true if a cycle action should be performed.
     */
    public boolean evaluate(long fileSize, int nTuples, T tuple) {
        return (this.fileSize > 0 && fileSize > this.fileSize)
                || (cntTuples > 0 && nTuples > 0 && nTuples % cntTuples == 0)
                || (tuplePredicate != null && tuplePredicate.test(tuple));
    }
    
    @Override
    public String toString() {
        return String.format("fileSize:%d cntTuples:%d periodMsec:%d tuplePredicate:%s",
                getFileSize(), getCntTuples(), getPeriodMsec(),
                getTuplePredicate() == null ? "no" : "yes");
    }
}
