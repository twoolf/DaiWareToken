/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/
package org.apache.edgent.streamscope;

import java.util.List;
import java.util.concurrent.TimeUnit;

import org.apache.edgent.streamscope.mbeans.StreamScopeMXBean;

import com.google.gson.Gson;

/**
 * Implementation of {@link StreamScopeMXBean}.
 * 
 * @see StreamScopeRegistryBean
 */
public class StreamScopeBean implements StreamScopeMXBean {
  private final StreamScope<?> streamScope;
  
  public StreamScopeBean(StreamScope<?> streamScope) {
    this.streamScope = streamScope;
  }

  @Override
  public boolean isEnabled() {
    return streamScope.isEnabled();
  }

  @Override
  public void setEnabled(boolean isEnabled) {
    streamScope.setEnabled(isEnabled);
  }

  @Override
  public boolean isPaused() {
    return streamScope.triggerMgr().isPaused();
  }

  @Override
  public void setPaused(boolean paused) {
    streamScope.triggerMgr().setPaused(paused);
  }

  @Override
  public String getSamples() {
    List<?> samples = streamScope.getSamples();
    Gson gson = new Gson();
    String json = gson.toJson(samples);
    return json;
  }

  @Override
  public int getSampleCount() {
    return streamScope.getSampleCount();
  }

  @Override
  public void setMaxRetentionCount(int maxCount) {
    streamScope.bufferMgr().setMaxRetentionCount(maxCount);
  }

  @Override
  public void setMaxRetentionTime(long age, TimeUnit unit) {
    streamScope.bufferMgr().setMaxRetentionTime(age, unit);
  }

  @Override
  public void setCaptureByCount(int count) {
    streamScope.triggerMgr().setCaptureByCount(count);
  }

  @Override
  public void setCaptureByTime(long elapsed, TimeUnit unit) {
    streamScope.triggerMgr().setCaptureByTime(elapsed, unit);
  }  
  
}
