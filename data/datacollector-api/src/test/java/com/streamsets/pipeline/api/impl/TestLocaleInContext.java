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

import org.junit.Before;
import org.junit.Test;

import java.util.Locale;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

public class TestLocaleInContext {

  @Before
  public void cleanUp() {
    LocaleInContext.set(null);
  }

  @Test
  public void testFirstUseInThread() throws Exception {
    ExecutorService executor = Executors.newSingleThreadExecutor();
    Callable<Boolean> tester = new Callable<Boolean>() {
      @Override
      public Boolean call() throws Exception {
        return Locale.getDefault().equals(LocaleInContext.get());
      }
    };

    Future<Boolean> future = executor.submit(tester);
    assertTrue(future.get());
    executor.shutdownNow();
  }

  @Test
  public void testSet() {
    Locale locale = Locale.forLanguageTag("xyz");
    LocaleInContext.set(locale);
    assertEquals(locale, LocaleInContext.get());
    LocaleInContext.set(null);
    assertEquals(Locale.getDefault(), LocaleInContext.get());
  }

}
