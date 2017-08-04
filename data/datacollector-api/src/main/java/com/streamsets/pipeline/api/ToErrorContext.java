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
 * Describes methods for various context classes that deals with error handling.
 */
public interface ToErrorContext {

  /**
   * Sends a record to the pipeline error stream with an associated <code>Exception</code>
   *
   * @param record the record to send to the error stream.
   * @param exception the associated <code>Exception</code>.
   */
  public void toError(Record record, Exception exception);

  /**
   * Sends a record to the pipeline error stream with an associated non-localizable error message.
   *
   * @param record the record to send to the error stream.
   * @param errorMessage the non-localizable error message.
   */
  public void toError(Record record, String errorMessage);

  /**
   * Sends a record to the pipeline error stream with an associated error code.
   *
   * @param record the record to send to the error stream.
   * @param errorCode the error code to for the record.
   * @param args the arguments for the <code>ErrorCode</code> message template.
   */
  public void toError(Record record, ErrorCode errorCode, Object... args);

}
