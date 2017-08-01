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

/**
 * FileWriter finalized (non-active) file retention configuration control.
 * <p>
 * File removal can be any combination of:
 * <ul>
 * <li>remove a file when {@code fileCount} would be exceeded</li>
 * <li>remove a file when {@code aggregateFileSize} would be exceeded</li>
 * <li>remove a file that's older than {@code ageSec} seconds</li>
 * </ul>
 */
public class FileWriterRetentionConfig {
    private int fileCount;
    private long aggregateFileSize;
    private long ageSec;
    private long periodMsec;
    
    /** same as {@code newConfig(fileCount, 0, 0, 0)}
     * 
     * @param fileCount remove a file when {@code fileCount} would be exceeded. 0 to disable.
     * @return the retention config
     */
    public static  FileWriterRetentionConfig newFileCountBasedConfig(int fileCount) {
        if (fileCount < 1)
            throw new IllegalArgumentException("fileCount");
        return newConfig(fileCount, 0, 0, 0);
    }
    /** same as {@code newConfig(0, aggregateFileSize, 0, 0)}
     * 
     * @param aggregateFileSize remove a file when {@code aggregateFileSize} would be exceeded. 0 to disable.
     * @return the retention config
     */
    public static  FileWriterRetentionConfig newAggregateFileSizeBasedConfig(long aggregateFileSize) {
        if (aggregateFileSize < 1)
            throw new IllegalArgumentException("aggregateFileSize");
        return newConfig(0, aggregateFileSize, 0, 0);
    }
    /** same as {@code newConfig(0, 0, ageSe, periodMsecc)}
     * 
     * @param ageSec remove a file that's older than {@code ageSec} seconds.  0 to disable.
     * @param periodMsec frequency for checking for ageSec based removal. 0 to disable.]
     * @return the retention config
     */
    public static  FileWriterRetentionConfig newAgeBasedConfig(long ageSec, long periodMsec) {
        if (ageSec < 1)
            throw new IllegalArgumentException("ageSec");
        if (periodMsec < 1)
            throw new IllegalArgumentException("periodMsec");
        return newConfig(0, 0, ageSec, periodMsec);
    }
    
    /**
     * Create a new configuration.
     * 
     * @param fileCount remove a file when {@code fileCount} would be exceeded. 0 to disable.
     * @param aggregateFileSize remove a file when {@code aggregateFileSize} would be exceeded. 0 to disable.
     * @param ageSec remove a file that's older than {@code ageSec} seconds.  0 to disable.
     * @param periodMsec frequency for checking for ageSec based removal. 0 to disable.]
     * @return the retention config
     */
    public static FileWriterRetentionConfig newConfig(int fileCount, long aggregateFileSize, long ageSec, long periodMsec) {
        return new FileWriterRetentionConfig(fileCount, aggregateFileSize, ageSec, periodMsec);
    }
    
    private FileWriterRetentionConfig(int fileCount, long aggregateFileSize, long ageSec, long periodMsec) {
        if (fileCount < 0)
            throw new IllegalArgumentException("fileCount");
        if (aggregateFileSize < 0)
            throw new IllegalArgumentException("aggregateFileSize");
        if (ageSec < 0)
            throw new IllegalArgumentException("ageSec");
        if (periodMsec < 0)
            throw new IllegalArgumentException("periodMsec");
        if (fileCount==0 && aggregateFileSize==0 && (ageSec==0 || periodMsec==0))
            throw new IllegalArgumentException("no retention configuration specified");
        this.fileCount = fileCount;
        this.aggregateFileSize = aggregateFileSize;
        this.ageSec = ageSec;
        this.periodMsec = periodMsec;
    }
    
    /**
     * Get the file count configuration value.
     * @return the value
     */
    public int getFileCount() { return fileCount; }
    
    /**
     * Get the aggregate file size configuration value.
     * @return the value
     */
    public long getAggregateFileSize() { return aggregateFileSize; }
    
    /**
     * Get the file age configuration value.
     * @return the value
     */
    public long getAgeSec() { return ageSec; }
    
    /**
     * Get the time period configuration value.
     * @return the value
     */
    public long getPeriodMsec() { return periodMsec; }
    
    /**
     * Evaluate if the specified values indicate that a final file should
     * be removed.
     *
     * @param fileCount the current number of retained files
     * @param aggregateFileSize the aggregate size of all of the retained files
     * @return true if a retained file should be removed.
     */
    public boolean evaluate(int fileCount, long aggregateFileSize) {
        return (this.fileCount > 0 && fileCount > this.fileCount)
                || (this.aggregateFileSize > 0 && aggregateFileSize > this.aggregateFileSize);
    }
    
    @Override
    public String toString() {
        return String.format("fileCount:%d aggSize:%d ageSec:%d",
                getFileCount(), getAggregateFileSize(), getAgeSec());
    }
    
}
