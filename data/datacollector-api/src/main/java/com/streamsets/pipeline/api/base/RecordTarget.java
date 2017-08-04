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
import com.streamsets.pipeline.api.Record;
import com.streamsets.pipeline.api.StageException;
import com.streamsets.pipeline.api.impl.Utils;

import java.util.Iterator;

/**
 * The <code>RecordTarget</code> is an convenience {@link com.streamsets.pipeline.api.Target} that handles
 * one record at the time (instead of batch) and has a built-in record error handling for the stage 'on record error'
 * configuration.
 */
public abstract class RecordTarget extends BaseTarget {

  /**
   * Writes the batch by calling the {@link #write(Record)} method for each record in the batch.
   * <p/>
   * If the calls to the {@link #write(Record)} throws an {@link OnRecordErrorException}, the error
   * handling is done based on the stage 'on record error' configuration, discarded, sent to error, or stopping the
   * pipeline.
   *
   * @param batch the batch of records to write.
   * for them to be available to the rest of the pipeline.
   * @throws StageException if the <code>Target</code> had an error while writing records.
   */
  @Override
  public void write(Batch batch) throws StageException {
    Iterator<Record> it = batch.getRecords();
    if (it.hasNext()) {
      while (it.hasNext()) {
        Record record = it.next();
        try {
          write(record);
        } catch (OnRecordErrorException ex) {
          switch (getContext().getOnErrorRecord()) {
            case DISCARD:
              break;
            case TO_ERROR:
              getContext().toError(record, ex);
              break;
            case STOP_PIPELINE:
              throw ex;
            default:
              throw new IllegalStateException(Utils.format("It should never happen. OnError '{}'",
                                                           getContext().getOnErrorRecord(), ex));
          }
        }
      }
    } else {
      emptyBatch();
    }
  }

  /**
   * Writes one record.
   * <p/>
   *
   * @param record the record to write.
   * for them to be available to the rest of the pipeline.
   * @throws StageException if the <code>Target</code> had an error while processing records.
   * @throws OnRecordErrorException if the <code>Record</code> cannot be processed correctly. The handling of this
   * exception will be base on the stage 'on record error' configuration
   */
  protected abstract void write(Record record) throws StageException;

  /**
   * Called if the batch to write does not have any records to allow the <code>RecordTarget</code> to do a special
   * handling in those situations.
   * <p/>
   * This implementation is a no-operation.
   *
   * @throws StageException
   */
  protected void emptyBatch() throws StageException {
  }

}
