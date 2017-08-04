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
package com.streamsets.pipeline.api.base;

import com.streamsets.pipeline.api.Batch;
import com.streamsets.pipeline.api.BatchMaker;
import com.streamsets.pipeline.api.StageException;
import com.streamsets.pipeline.api.Record;

import java.util.List;

/**
 * The <code>SingleProcessor</code> is an convenience {@link com.streamsets.pipeline.api.Processor} that handles
 * writes all records to a single output stream.
 */
public abstract class SingleLaneProcessor extends BaseProcessor {

  /**
   * Data Collector({@link SingleLaneProcessor}) stages receive an instance of a
   * <code>SingleLaneBatchMaker</code> to write to the pipeline the records they create or process.
   */
  public interface SingleLaneBatchMaker {

    /**
     * Adds a record to the <code>SingleLaneBatchMaker</code>.
     *
     * @param record the record to add.
     */
    public void addRecord(Record record);
  }

  private String outputLane;

  /**
   * Constructor.
   */
  public SingleLaneProcessor() {
    setRequiresSuperInit();
  }

  /**
   * Performs the <code>SingleLaneProcessor</code> required initialization. Subclasses overriding this method must
   * call <code>super.init()</code>.
   *
   * @return the list of configuration issues found during initialization, an empty list if none.
   */
  @Override
  protected List<ConfigIssue> init() {
    List<ConfigIssue> issues = super.init();
    if (getContext().getOutputLanes().size() != 1) {
      issues.add(getContext().createConfigIssue(null, null, Errors.API_00, getInfo().getInstanceName(),
                                                getContext().getOutputLanes().size()));
    } else {
      outputLane = getContext().getOutputLanes().iterator().next();
    }
    setSuperInitCalled();
    return issues;
  }

  /**
   * Processes the batch by calling the {@link #process(Batch, SingleLaneBatchMaker)} method.
   * <p/>
   *
   * @param batch the batch of records to process.
   * @param batchMaker records created by the <code>Processor</code> stage must be added to the <code>BatchMaker</code>
   * for them to be available to the rest of the pipeline.
   * @throws StageException if the <code>Processor</code> had an error while processing records.
   */
  @Override
  public void process(final Batch batch, final BatchMaker batchMaker) throws StageException {
    SingleLaneBatchMaker slBatchMaker = new SingleLaneBatchMaker() {
      @Override
      public void addRecord(Record record) {
        batchMaker.addRecord(record, outputLane);
      }
    };
    process(batch, slBatchMaker);
  }

  /**
   * When running a pipeline, this method is called to process a batch of records.
   *
   * @param batch the batch of records to process.
   * @param singleLaneBatchMaker records created by the <code>Processor</code> stage must be added to the
   * <code>SingleLaneBatchMaker</code> for them to be available to the rest of the pipeline.
   * @throws StageException if the <code>Processor</code> had an error while processing records.
   */
  public abstract void process(Batch batch, SingleLaneBatchMaker singleLaneBatchMaker) throws StageException;

}
