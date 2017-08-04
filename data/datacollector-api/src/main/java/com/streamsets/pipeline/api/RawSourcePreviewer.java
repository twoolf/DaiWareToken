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

import java.io.InputStream;

/**
 * Interface that provides raw preview capabilities to Data Collector origin ({@link Source} stages via the
 * {@link RawSource} annotation.
 */
public interface RawSourcePreviewer {

  /**
   * Returns the <code>InputStream</code> containing raw data for preview.
   *
   * @param maxLength returns the maximum length for the returned input stream.
   * @return the <code>InputStream</code> containing raw data for preview.
   */
  InputStream preview(int maxLength);

  /**
   * Returns the MIME type of the raw data.
   *
   * @return the MIME type of the raw data.
   */
  String getMimeType();

  /**
   * Sets the MIME type of the raw data.
   *
   * @param mimeType the MIME type of the raw data.
   */
  void setMimeType(String mimeType);

}
