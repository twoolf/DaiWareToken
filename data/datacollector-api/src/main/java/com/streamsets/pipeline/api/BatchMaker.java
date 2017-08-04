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

import java.util.List;

/**
 * Data Collector origin ({@link Source}) and processor ({@link Processor}) stages receive an instance of a
 * <code>BatchMaker</code> to write to the pipeline the records they create or process.
 */
public interface BatchMaker {

  /**
   * Returns the available lane names (stream names) for the stage.
   *
   * @return the available lane names (stream names) for the stage.
   */
  public List<String> getLanes();

  /**
   * Adds a record to the <code>BatchMaker</code>.
   *
   * @param record the record to add.
   * @param lanes the lane(s)/stream(s) to add the record to. If the stage has a single output lane there is no need
   * to specify the lane name.
   */
  public void addRecord(Record record, String... lanes);

}
