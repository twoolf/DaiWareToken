/*
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

package com.streamsets.pipeline.api.lineage;

import com.streamsets.pipeline.api.Label;

/**
 * These are specific attributes - each LineageEventType needs a subset of these...
 */
public enum LineageSpecificAttribute implements Label {



  DESCRIPTION("description"),

  // see EndPointType for a list.
  ENDPOINT_TYPE("endPointType"),

  // entity we're working on, perhaps a file name.
  ENTITY_NAME("entityName"),

  // tags to add to the Lineage Metadata
  TAGS("tags"),
  // properties (key, value pairs) to add to the Lineage Metadata
  PROPERTIES("properties"),

  ;

  private String label;

  LineageSpecificAttribute(String label){
    this.label = label;
  }

  @Override
  public String getLabel() {
    return label;
  }
}
