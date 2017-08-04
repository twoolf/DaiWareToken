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

import java.util.Arrays;
import java.util.List;

import static com.streamsets.pipeline.api.lineage.LineageSpecificAttribute.DESCRIPTION;
import static com.streamsets.pipeline.api.lineage.LineageSpecificAttribute.ENDPOINT_TYPE;
import static com.streamsets.pipeline.api.lineage.LineageSpecificAttribute.ENTITY_NAME;

public enum LineageEventType implements Label {
  START(
      "START",
      true,
      Arrays.asList(
          DESCRIPTION
      )),
  STOP(
      "STOP",
      true,
      Arrays.asList(
          DESCRIPTION
      )),
  ENTITY_CREATED(
      "ENTITY_CREATED",
      false,
      Arrays.asList(
          ENDPOINT_TYPE,
          ENTITY_NAME,
          DESCRIPTION
      )),
  ENTITY_READ(
      "ENTITY_READ",
      false,
      Arrays.asList(
          ENDPOINT_TYPE,
          ENTITY_NAME,
          DESCRIPTION
      )),
  ENTITY_WRITTEN(
      "ENTITY_WRITTEN",
      false,
      Arrays.asList(
          ENDPOINT_TYPE,
          ENTITY_NAME,
          DESCRIPTION
      )),
  ;
  private String label;
  private boolean frameworkOnly;
  private List<LineageSpecificAttribute> specificAttributes;

  LineageEventType(String label, boolean frameworkOnly, List<LineageSpecificAttribute> specificAttributes){
    this.label = label;
    this.frameworkOnly = frameworkOnly;
    this.specificAttributes = specificAttributes;
  }

  @Override
  public String getLabel() {
    return label;
  }

  public boolean isFrameworkOnly() {
    return frameworkOnly;

  }

  public List<LineageSpecificAttribute> getSpecificAttributes() {
    return specificAttributes;
  }

  public boolean isOneOf(LineageEventType... types) {
    if (types == null) {
      return false;
    }

    for (LineageEventType t : types) {
      if (this == t) {
        return true;
      }
    }

    return false;
  }
}
