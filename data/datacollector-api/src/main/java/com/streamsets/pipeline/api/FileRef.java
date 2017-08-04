/**
 * Copyright 2017 StreamSets Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.streamsets.pipeline.api;

import java.io.IOException;
import java.util.Set;

/**
 * Class which defines a reference to a file through
 * which streams can be created to the file.
 */
public abstract class FileRef {
  private final int bufferSize;

  public FileRef(int bufferSize) {
    this.bufferSize = bufferSize;
  }
  /**
   * Defines the supported stream classes for this {@link FileRef}.
   * The minimal requirement is that this should contain
   * at least {@link java.io.InputStream}
   * @param <T> Stream Implementation of {@link AutoCloseable}
   * @return the supported stream classes.
   */
  public abstract <T extends AutoCloseable> Set<Class<T>> getSupportedStreamClasses();

  /**
   * Creates the Stream instance based on the stream class type.
   * @param context the context of the stage creating the stream.
   * @param streamClassType the stream class type
   * @param <T> Stream Implementation of {@link AutoCloseable}
   * @return the stream
   * @throws IOException if there are issues in creating the stream.
   */
  public abstract <T extends AutoCloseable> T createInputStream(
      Stage.Context context,
      Class<T> streamClassType
  ) throws IOException;

  /**
   * Returns the buffer size of the returned input stream.
   * @return The buffer size used by the input stream, return -1, if there is no way to determine this.
   */
  public int getBufferSize() {
    return this.bufferSize;
  }
}
