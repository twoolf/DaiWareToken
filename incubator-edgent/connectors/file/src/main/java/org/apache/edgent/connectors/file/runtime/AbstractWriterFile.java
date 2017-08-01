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

package org.apache.edgent.connectors.file.runtime;

import java.io.IOException;
import java.nio.file.Path;

/**
 * Generic class for writing of tuples to a file.
 * <p>
 * The class is not responsible for flush strategy, finalize strategy, etc
 */
abstract class AbstractWriterFile<T> {
    private final Path path;
    protected long size;
    private long tupleCnt;
    public AbstractWriterFile(Path path) {
        this.path = path;
    }
    public Path path() { return path; }
    public long size() { return size; }
    public long tupleCnt() { return tupleCnt; }
    public abstract void flush() throws IOException;
    public abstract void close() throws IOException;
    /** do what's needed to write the tuple */
    protected abstract int writeTuple(T tuple) throws IOException;
    /**
     * @param tuple the tuple to write 
     * @return the number of bytes written
     * @throws IOException on failure
     */
    public int write(T tuple) throws IOException {
        tupleCnt++;
        int nbytes = writeTuple(tuple);
        size += nbytes;
        return nbytes;
    }
}
