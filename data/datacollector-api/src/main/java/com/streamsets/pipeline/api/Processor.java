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
 * A <code>Processor</code> is a Data Collector processor stage. Processor stages receive records from an origin
 * ({@link Source}) or other processors stages, perform operations on the records and write them out so they can be
 * processed by another processor or destination ({@link Target}) stages.
 *
 * @see Source
 * @see Target
 */
public interface Processor extends Stage<Processor.Context> {

  /**
   * <code>Processor</code> stage context.
   */
  public interface Context extends Stage.Context, ToErrorContext, ToEventContext {

    /**
     * Returns the output lane names (stream names) of the <code>Source</code>.
     *
     * @return the output lane names (stream names) of the <code>Source</code>.
     */
    public List<String> getOutputLanes();

    /**
     * Creates an empty record indicating another record as its source.
     *
     * @param originatorRecord the original record.
     * @return an empty record with the specified ID and raw data.
     */
    public Record createRecord(Record originatorRecord);

    /**
     * Creates an empty record indicating another record as its source.
     *
     * @param originatorRecord the original record.
     * @param sourceIdPostfix the sourceId postfix
     * @return an empty record with the specified ID and raw data.
     * The sourceIdPostFix will be appended to the sourceId of originator record.
     */
    public Record createRecord(Record originatorRecord, String sourceIdPostfix);

    /**
     * Creates an empty record indicating another record as its source, including the original raw data of the record.
     *
     * @param originatorRecord the original record.
     * @param raw the record raw data.
     * @param rawMime the MIME type of the raw data.
     * @return an empty record with the specified ID and raw data.
     */
    public Record createRecord(Record originatorRecord, byte[] raw, String rawMime);

    /**
     * Clones a record.
     *
     * @param record the record to clone.
     * @return the cloned record. The cloned record is a share-nothing deep-copy of the original record.
     */
    public Record cloneRecord(Record record);


    /**
     * Clones a record.
     *
     * @param record the record to clone.
     * @param sourceIdPostfix the sourceId postfix
     * @return the cloned record. The cloned record is a share-nothing deep-copy of the original record.
     * The sourceIdPostFix will be appended to the sourceId of original record being cloned.
     */
    public Record cloneRecord(Record record, String sourceIdPostfix);
  }

  /**
   * When running a pipeline, the Data Collector calls this method from the <code>Processor</code> stage with a
   * batch of records to process.
   *
   * @param batch the batch of records to process.
   * @param batchMaker records created by the <code>Processor</code> stage must be added to the <code>BatchMaker</code>
   * for them to be available to the rest of the pipeline.
   * @throws StageException if the <code>Processor</code> had an error while processing records.
   */
  public void process(Batch batch, BatchMaker batchMaker) throws StageException;

}
