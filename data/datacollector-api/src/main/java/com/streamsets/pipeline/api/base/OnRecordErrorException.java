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

import com.streamsets.pipeline.api.ErrorCode;
import com.streamsets.pipeline.api.Record;
import com.streamsets.pipeline.api.StageException;

/**
 * Exception a stage can throw when a specific record should go to the error pipeline.
 * <p/>
 * A record should be considered in error if it is un-parseable or may be missing required information.
 * <p/>
 * This should not be thrown due to issues such as destination connectivity, etc. Those error conditions
 * should be considered a {@link StageException} in which the pipeline should stop in an error state.
 */
public class OnRecordErrorException extends StageException {
  private final transient Record record;

  /**
   * Class constructor specifying the error code and any parameters to be interpolated into the error message.
   *
   * @param errorCode ErrorCode describing the problem
   * @param params Parameters to be interpolated into the ErrorCode's error message
   */
  public OnRecordErrorException(ErrorCode errorCode, Object... params) {
    this(null, errorCode, params);
  }

  /**
   * Optional class constructor which also specifies the record in error. Useful when error records
   * must be batched up prior to handling.
   *
   * @param record Record which caused the error
   * @param errorCode ErrorCode describing the problem
   * @param params Parameters to be interpolated into the ErrorCode's error message
   */
  public OnRecordErrorException(Record record, ErrorCode errorCode, Object... params) {
    super(errorCode, params);
    this.record = record;
  }

  /**
   * Getter for the record which caused the error.
   * @return error record
   */
  public Record getRecord() {
    return record;
  }
}
