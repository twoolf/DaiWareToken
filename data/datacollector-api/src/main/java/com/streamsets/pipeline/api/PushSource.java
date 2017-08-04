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

import java.util.Map;

/**
 * A <code>PushSource</code> is a type of Data Collector origin stage that consumes or listen for incoming data and
 * pushes them down to({@link Processor}) or destination ({@link Target}) stages.
 *
 * Certain methods in Context that deals with records such as toError() or toEvent() will work only in thread that is
 * currently in batch context - e.g. after startBatch() call and before processBatch() is finished.
 *
 * Unlike Source that keeps one single dimensional offset, the framework is keeping a two dimensional offset for
 * PushSource. Internally the offset is represented as a map where key is origin driven entity name (table name, file
 * name, topic+offset, ...) and value is offset within the given entity (offset in given table, offset in given
 * file, ...).
 *
 * @see Source
 * @see ProtoSource
 */
public interface PushSource extends ProtoSource<PushSource.Context> {

  /**
   * <code>PushSource</code> stage context.
   */
  public interface Context extends ProtoSource.Context {

    /**
     * Start new batch and return it's context that allows source to create batch maker that can further be used
     * to pass records for the rest of the pipeline to process.
     *
     * This method is thread safe.
     *
     * @return Context object representing new batch
     */
    public BatchContext startBatch();

    /**
     * Process given batch - run it through rest of the pipeline. The method returns true
     * if and only if the data reached all destinations properly, otherwise it returns false.
     * Source can use this to for example properly respond to HTTP call with error status.
     *
     * This is a blocking call, the execution will wait until a pipeline runner is available.
     *
     * This method is thread safe.
     *
     * This method will not commit any offsets. If used, it's origin's responsibility to call method
     * commitOffset() to commit offsets when appropriate. If you need to commit offset after every batch
     * consider using processBatch(BatchContext, String, String) instead.
     *
     * @param batchContext Batch to be passed to the pipeline.
     * @return true if and only if the batch has reached all destinations
     */
    public boolean processBatch(BatchContext batchContext);

    /**
     * Process given batch - run it through rest of the pipeline. The method returns true
     * if and only if the data reached all destinations properly, otherwise it returns false.
     * Source can use this to for example properly respond to HTTP call with error status.
     *
     * This is a blocking call, the execution will wait until a pipeline runner is available.
     *
     * Upon execution it will automatically commit given entityOffset for given entityName.
     *
     * This method is thread safe.
     *
     * @param batchContext Batch to be passed to the pipeline.
     * @param entityName Name of the origin driven entity (file name, topic name, ...). Can't be NULL.
     * @param entityOffset String representation of the offset for given entity. Null value will remove the entity
     *                     from tracking structures.
     * @return true if and only if the batch has reached all destinations
     */
    public boolean processBatch(BatchContext batchContext, String entityName, String entityOffset);

    /**
     * Registers offset for given origin driven entity.
     *
     * The offset is persisted between pipeline executions. This methods works well with processBatch(BatchContext)
     * for origins that have advanced use cases for keeping non-trivial offsets. Considering using method
     * processBatch(BatchContext, String, String) if you need to commit offset after every batch.
     *
     * This method is thread safe.
     *
     * @param entityName Name of the origin driven entity (file name, topic name, ...)
     * @param entityOffset String representation of the offset for given entity
     */
    public void commitOffset(String entityName, String entityOffset);

    /**
     * Returns configured delivery guarantee for this pipeline.
     *
     * If the origin is doing commit of data on it's own - e.g. using the method processBatch(BatchContext), then this
     * helps origin make decision whether to commit when the function returned false. Push origins that are using
     * framework to keep offsets, e.g. method processBatch(BatchContext, String, String) does not need to use this method
     * as the framework will commit offset automatically per the configuration.
     *
     * @return DeliveryGuarantee for this pipeline.
     */
    public DeliveryGuarantee getDeliveryGuarantee();
  }

  /**
   * Returns the ideal number of threads that the source would like to run. Data Collector
   * will use this information to create sufficiently large pipeline runner pool.
   *
   * @return Expected number of threads
   */
  public int getNumberOfThreads();

  /**
   * When a pipeline is initialized and prepared to run, the Data Collector calls this method to start the Source.
   *
   * When this method returns the pipeline transitions to stopped state. Use methods in the Context to create batches
   * of data and propagate them to Data Collector.
   *
   * @param lastOffsets Immutable Map with all committed entities and their representative offsets.
   * @param maxBatchSize the requested maximum batch size that a single call to Context.processBatch should produce
   * @throws StageException if the <code>PushSource</code> had an error while consuming data or creating records.
   */
  void produce(Map<String, String> lastOffsets, int maxBatchSize) throws StageException;
}
