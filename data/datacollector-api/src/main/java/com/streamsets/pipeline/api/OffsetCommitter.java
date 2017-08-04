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

/**
 * {@link Source}s that keep track of the data offset on their own should implement this interface.
 * <p/>
 * The Data Collector will not keep track of the offset. This interface is applicable only to {@link Source} and not
 * to the {@link PushSource}.
 */
public interface OffsetCommitter {

  /**
   * Invoked by the Data Collector after the completion of each batch to signal the <code>Source</code> that the
   * batch has been fully processed.
   * <p/>
   *
   * @param offset the offset of the batch being committed, this is the same offset the <code>Source</code> returned
   * when producing the batch.
   * @throws StageException if there was an error committing the batch.
   */
  public void commit(String offset) throws StageException;

}
