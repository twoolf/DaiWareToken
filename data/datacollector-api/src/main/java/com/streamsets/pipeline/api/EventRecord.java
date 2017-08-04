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
 * Event record is a special Record instance that have few additional required
 * header fields like type, version and creation_time.
 */
public interface EventRecord extends Record {

  /**
   * Type of the event, it's value is defined by generating stage.
   */
  public static final String TYPE = "sdc.event.type";

  /**
   * Each event type is versioned separately. This is also defined by generating stage.
   */
  public static final String VERSION = "sdc.event.version";

  /**
   * Timestamp of the time when the event was generated.
   */
  public static final String CREATION_TIMESTAMP = "sdc.event.creation_timestamp";

}
