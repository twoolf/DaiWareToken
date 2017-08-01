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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.edgent.execution.services.ControlService;
import org.apache.edgent.streamscope.mbeans.StreamScopeMXBean;
import org.apache.edgent.streamscope.mbeans.StreamScopeRegistryMXBean;

/**
 * An implementation of {@link StreamScopeRegistryMXBean}.
 */
public class StreamScopeRegistryBean implements StreamScopeRegistryMXBean {
  private final StreamScopeRegistry rgy;
  private final ControlService cs;
  
  // streamId -> controlId for StreamScopeBean registered for streamId
  // TODO: seems like if we can cs.getControl(type,alias) to find a control
  // then there should be an cs.unregister(type,alias).
  // lacking that we need to record controls that were registered.
  private final Map<String,String> controlIdMap = new HashMap<>();
  
  public StreamScopeRegistryBean(StreamScopeRegistry rgy, ControlService cs) {
    this.rgy = rgy;
    this.cs = cs;
  }
  
  @Override
  public StreamScopeMXBean lookup(String jobId, String opletId, int oport) {
    // lazy-register the mbeans
    String streamId = StreamScopeRegistry.mkStreamId(jobId, opletId, oport);
    StreamScopeMXBean mbean = cs.getControl(StreamScopeMXBean.TYPE, streamId, StreamScopeMXBean.class);
    
    if (mbean == null) {
      String name = StreamScopeRegistry.nameForStreamId(streamId);
      StreamScope<?> ss = rgy.lookup(name);
      if (ss != null) {
        mbean = new StreamScopeBean(ss);
        String controlId = cs.registerControl(StreamScopeMXBean.TYPE, 
            StreamScopeMXBean.TYPE+streamId, streamId,
            StreamScopeMXBean.class, mbean);
        controlIdMap.put(name, controlId);
      }
    }
    
    return mbean;
  }
  
  /**
   * Unregister all StreamScopeMXBean for the oplet.
   * no-op if none registered.
   * <BR>
   * N.B. This does not unregister StreamScopes from the underlying StreamScopeRegistry.
   * @param jobId
   * @param opletId
   */
  void unregister(String jobId, String opletId) {
    for (String controlId : controlIds(jobId, opletId)) {
      cs.unregister(controlId);
    }
  }
  
  private List<String> controlIds(String jobId, String opletId) {
    String namePrefix = StreamScopeRegistry.mkStreamIdNamePrefix(jobId, opletId);
    List<String> controlIds = new ArrayList<>();
    for (String streamId : controlIdMap.keySet()) {
      if (streamId.startsWith(namePrefix))
        controlIds.add(controlIdMap.get(streamId));
    }
    return controlIds;
  }
  
}

