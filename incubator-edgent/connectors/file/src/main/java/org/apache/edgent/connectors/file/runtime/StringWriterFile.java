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

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;

import org.slf4j.Logger;

class StringWriterFile extends AbstractWriterFile<String> {
    private static Logger trace = FileConnector.getTrace();
    private BufferedWriter bw;
    private final Charset cs;

    public StringWriterFile(Path path, Charset cs) {
        super(path);
        this.cs = cs;
    }

    @Override
    protected int writeTuple(String tuple) throws IOException {
        if (bw == null) {
            trace.info("creating file {}", path());
            bw = Files.newBufferedWriter(path(), cs);
        }
        bw.write(tuple);
        bw.write("\n");
        // ugh. inefficient
        int nbytes = tuple.getBytes(cs).length;
        nbytes++;
        return nbytes;
    }

    @Override
    public void flush() throws IOException {
        if (bw != null) {
            trace.trace("flushing {}", path());
            bw.flush();
        }
    }

    @Override
    public void close() throws IOException {
        if (bw != null) {
            trace.info("closing {}", path());
            BufferedWriter bw = this.bw;
            this.bw = null;
            bw.close();
        }
    }
    
}
