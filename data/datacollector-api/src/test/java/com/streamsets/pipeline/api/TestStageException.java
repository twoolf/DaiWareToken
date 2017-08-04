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

import com.streamsets.pipeline.api.base.Errors;
import com.streamsets.pipeline.api.impl.LocaleInContext;
import com.streamsets.pipeline.api.impl.Utils;
import org.junit.After;
import org.junit.Assert;
import org.junit.Test;

import java.util.Locale;

public class TestStageException {

  @After
  public void cleanUp() {
    LocaleInContext.set(null);
  }

  @Test
  public void testException() {
    StageException ex = new StageException(Errors.API_00);
    Assert.assertEquals(Errors.API_00, ex.getErrorCode());
    Assert.assertEquals("API_00 - " + Errors.API_00.getMessage(), ex.getMessage());
    LocaleInContext.set(Locale.forLanguageTag("abc"));
    Assert.assertEquals("API_00 - " + Errors.API_00.getMessage(), ex.getMessage());
    LocaleInContext.set(Locale.forLanguageTag("xyz"));
    Assert.assertEquals("API_00 - Hello XYZ '{}'", ex.getLocalizedMessage());
    LocaleInContext.set(null);
    Assert.assertNull(ex.getCause());

    Exception cause = new Exception();
    ex = new StageException(Errors.API_00, cause);
    Assert.assertEquals(cause, ex.getCause());

    ex = new StageException(Errors.API_00, "a");
    Assert.assertEquals("API_00 - " + Utils.format(Errors.API_00.getMessage(), "a"), ex.getMessage());
    LocaleInContext.set(Locale.forLanguageTag("xyz"));
    Assert.assertEquals("API_00 - Hello XYZ 'a'", ex.getLocalizedMessage());
    LocaleInContext.set(null);
    Assert.assertNull(ex.getCause());
  }

  @Test
  public void testStacktrace() {
    Exception cause = new Exception();
    StageException ex = new StageException(Errors.API_00, "a", 1, cause);
    Assert.assertEquals(
        "API_00 - " + Utils.format(Errors.API_00.getMessage(), "a", 1),
        ex.getMessage());

    LocaleInContext.set(Locale.forLanguageTag("xyz"));
    Assert.assertEquals("API_00 - Hello XYZ 'a'", ex.getLocalizedMessage());
    LocaleInContext.set(null);
    Assert.assertEquals(cause, ex.getCause());

  }

  @Test
  public void testInitCause() {
    StageException ex = new StageException(Errors.API_00);
    Exception cause = new IllegalArgumentException();
    ex.initCause(cause);
    Assert.assertEquals(cause, ex.getCause());
  }

}
