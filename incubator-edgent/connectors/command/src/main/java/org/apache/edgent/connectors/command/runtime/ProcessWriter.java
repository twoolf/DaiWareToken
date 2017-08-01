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
package org.apache.edgent.connectors.command.runtime;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;

import org.apache.edgent.function.Consumer;

/**
 * A {@code Consumer<String>>} to receive data and write it to a process's input.
 * <P>
 * Each write is followed by a flush() though that only helps to
 * reduce the time it takes to notice that a cmd has failed.
 * Supposedly "successfully written and flushed" values are not guaranteed to
 * have been received by a cmd.
 */
class ProcessWriter implements Consumer<String> {
  private static final long serialVersionUID = 1L;
  private final BufferedWriter writer;
  
  /**
   * Create a new consumer for UTF8 strings to write to a process's
   * {@link Process#getOutputStream() input}
   * 
   * @param process to process to write to.
   */
  ProcessWriter(Process process) {
    writer = new BufferedWriter(new OutputStreamWriter(
        process.getOutputStream(), StandardCharsets.UTF_8));
  }

  @Override
  public void accept(String value) {
    try {
      // see class doc regarding guarantees.
      writer.write(value);
      writer.newLine();
      writer.flush();
    }
    catch (IOException e) {
      CommandConnector.logger.error("Unable to write to cmd", e);
      // caller (CommandWriter) requires throw to detect failure and recover
      throw new RuntimeException("Unable to write to cmd", e);
    }
  }
  
}