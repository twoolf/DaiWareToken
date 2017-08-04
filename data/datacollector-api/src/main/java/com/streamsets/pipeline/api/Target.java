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
 * A <code>Target</code> is a Data Collector destination stage. Destination stages receive records from origin
 * ({@link Source}) or processor ({@link Processor}) stages and write them to an external system.
 *
 * @see Source
 * @see Processor
 */
public interface Target extends Stage<Target.Context> {

  /**
   * <code>Target</code> stage context.
   */
  public interface Context extends Stage.Context, ToErrorContext, ToEventContext {

  }

  /**
   * When running a pipeline, the Data Collector calls this method from the <code>Target</code> stage to write a batch
   * of records to an external system.
   * <p/>
   * @param batch the batch of record to write to the external system.
   * @throws StageException if the <code>Target</code> had an error while writing to the external system.
   */
  public void write(Batch batch) throws StageException;

}
