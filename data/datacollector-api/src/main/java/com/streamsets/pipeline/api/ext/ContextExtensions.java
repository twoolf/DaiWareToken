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
package com.streamsets.pipeline.api.ext;

import com.streamsets.pipeline.api.StageException;
import com.streamsets.pipeline.api.ext.json.Mode;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.Reader;
import java.io.Writer;
import java.util.List;

public interface ContextExtensions {

  RecordReader createRecordReader(InputStream inputStream, long initialPosition, int maxObjectLen)
      throws IOException;

  RecordWriter createRecordWriter(OutputStream outputStream) throws IOException;

  void notify(List<String> addresses, String subject, String body) throws StageException;

  Sampler getSampler();

  /* JSON Parser / Generator Abstractions */
  JsonObjectReader createJsonObjectReader(Reader reader, long initialPosition, int maxObjectLen, Mode mode, Class<?> objectClass) throws IOException;
  JsonObjectReader createJsonObjectReader(Reader reader, long initialPosition, Mode mode, Class<?> objectClass) throws IOException;
  JsonRecordWriter createJsonRecordWriter(Writer writer, Mode mode) throws IOException;
}
