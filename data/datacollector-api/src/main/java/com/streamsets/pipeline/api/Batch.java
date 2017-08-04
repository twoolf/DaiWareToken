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

import java.util.Iterator;

/**
 * Data Collector processor ({@link Processor}) and destination ({@link Target}) stages receive an instance of a
 * <code>Batch</code> that gives them access to the record in the current batch for processing.
 */
public interface Batch {

  /**
   * Returns source's entity that was used to generate data for this batch.
   * <p/>
   * This return value should be treated as an opaque value as it is source dependent.
   *
   * @return the entity name
   */
  public String getSourceEntity();

  /**
   * Returns the initial offset of the current batch.
   * <p/>
   * This return value should be treated as an opaque value as it is source dependent.
   *
   * @return the initial offset of the current batch.
   */
  public String getSourceOffset();

  /**
   * Returns an iterator with all the records in the batch for the current stage.
   * <p/>
   * Every time this method is called it returns a new iterator with all records in the batch.
   *
   * @return an iterator with all the records in the batch for the current stage.
   */
  public Iterator<Record> getRecords();

}
