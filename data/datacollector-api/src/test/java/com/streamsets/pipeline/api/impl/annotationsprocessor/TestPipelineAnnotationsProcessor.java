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
package com.streamsets.pipeline.api.impl.annotationsprocessor;

import org.junit.Assert;
import org.junit.Test;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

public class TestPipelineAnnotationsProcessor {

  @Test
  public void testToJson() {
    List<String> list = new ArrayList<>();
    Assert.assertEquals("[]", PipelineAnnotationsProcessor.toJson(list).replace("\n" ,"").replace(" " ,""));
    list.add("a");
    Assert.assertEquals("[\"a\"]", PipelineAnnotationsProcessor.toJson(list).replace("\n", "").replace(" ", ""));
    list.add("b");
    Assert.assertEquals("[\"a\",\"b\"]", PipelineAnnotationsProcessor.toJson(list).replace("\n", "").replace(" ", ""));
  }

  private String readResource(String resource) throws Exception {
    InputStream is = getClass().getClassLoader().getResourceAsStream(resource);
    Assert.assertNotNull(is);
    try (BufferedReader reader = new BufferedReader(new InputStreamReader(is))) {
      StringBuilder sb = new StringBuilder();
      String line = reader.readLine();
      while (line != null) {
        sb.append(line);
        line = reader.readLine();
      }
      return sb.toString();
    }
  }

  @Test
  public void testStagesResource() throws Exception {
    String resource = readResource(PipelineAnnotationsProcessor.STAGES_FILE);
    Assert.assertTrue(resource.contains(DummySource.class.getName()));
    Assert.assertTrue(resource.contains(DummyTarget.class.getName()));
  }

  @Test
  public void testElDefsResource() throws Exception {
    String resource = readResource(PipelineAnnotationsProcessor.ELDEFS_FILE);
    Assert.assertTrue(resource.contains(DummyELs.class.getName()));
  }

  @Test
  public void testBundlesResource() throws Exception {
    String resource = readResource(PipelineAnnotationsProcessor.BUNDLES_FILE);
    Assert.assertTrue(resource.contains(DummyELs.class.getName()));
    Assert.assertTrue(resource.contains(DummyTarget.class.getName()));
  }

  @Test
  public void testLineagePublisherResource() throws Exception {
    String resource = readResource(PipelineAnnotationsProcessor.LINEAGE_PUBLISHERS_FILE);
    Assert.assertTrue(resource.contains(DummyLineagePublisher.class.getName()));
  }

  @Test
  public void testCredentialStoreResource() throws Exception {
    String resource = readResource(PipelineAnnotationsProcessor.CREDENTIAL_STORE_FILE);
    Assert.assertTrue(resource.contains(DummyCredentialStore.class.getName()));
  }

}
