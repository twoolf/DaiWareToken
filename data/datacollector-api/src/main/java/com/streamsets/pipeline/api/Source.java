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
 * A <code>Source</code> is Data Collector origin stage. Origin stages consume data from an external system
 * creating records that can be processed by processor ({@link Processor}) or destination ({@link Target}) stages.
 *
 * @see Processor
 * @see Target
 */
public interface Source extends ProtoSource<Source.Context> {

  /**
   * Data collector is using a Map<String, String> to keep track of offsets. As this Source uses only one
   * dimensional String instead, the offset is kept in the map with given key. This constant is particularly
   * useful when you switched source from Source to PushSource and you need to get back the Source's version of
   * the offset.
   */
  public static final String POLL_SOURCE_OFFSET_KEY = "$com.streamsets.datacollector.pollsource.offset$";

  /**
   * <code>Source</code> stage context.
   */
  public interface Context extends ProtoSource.Context, ToErrorContext, ToEventContext {
  }

  /**
   * When running a pipeline, the Data Collector calls this method from the <code>Source</code> stage to obtain a batch
   * of records for processing.
   * <p/>
   * <code>Source</code> stages should not block indefinitely within this method if there is no data. They should have
   * an internal timeout after which they produce an empty batch. By doing so it gives the chance to other stages in
   * pipeline to know that the pipeline is still healthy but there is no data coming; and potentially allowing
   * notifications to external systems.
   *
   * @param lastSourceOffset the offset returned by the previous call to this method, or <code>NULL</code> if this
   * method is being called for the first time ever.
   * @param maxBatchSize the requested maximum batch size a single call to this method should produce.
   * @param batchMaker records created by the <code>Source</code> stage must be added to the <code>BatchMaker</code>
   * for them to be available to the rest of the pipeline.
   * @return the offset for the next call to this method. If <code>NULL</code> is returned it means the
   * <code>Source</code> stage has fully process that data, that no more data is to be expected and that the
   * pipeline should finish once the current batch is fully processed.
   * @throws StageException if the <code>Source</code> had an error while consuming data or creating records.
   */
  public String produce(String lastSourceOffset, int maxBatchSize, BatchMaker batchMaker) throws StageException;

}
