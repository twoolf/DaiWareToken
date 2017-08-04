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
package com.streamsets.pipeline.api.impl;

import com.streamsets.pipeline.api.FileRef;
import com.streamsets.pipeline.api.base.Errors;

public class FileRefTypeSupport extends TypeSupport<FileRef> {

  @Override
  public Object create(Object value) {
    return clone(value);
  }

  @Override
  public FileRef convert(Object value) {
    if (value instanceof FileRef) {
      return (FileRef)value;
    }
    throw new IllegalArgumentException(Utils.format(Errors.API_23.getMessage(), value.getClass().getName()));
  }
  @Override
  public Object convert(Object value, TypeSupport targetTypeSupport) {
    if (targetTypeSupport instanceof FileRefTypeSupport) {
      return value;
    }
    throw new IllegalArgumentException(Utils.format(Errors.API_24.getMessage(), targetTypeSupport));
  }

  @Override
  public Object clone(Object value) {
    return value;
  }
}
