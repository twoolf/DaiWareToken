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
package org.apache.edgent.streamscope.oplets;

import org.apache.edgent.function.Consumer;
import org.apache.edgent.oplet.OpletContext;
import org.apache.edgent.oplet.functional.Peek;
import org.apache.edgent.streamscope.StreamScopeRegistry;

/**
 * A Stream "oscilloscope" oplet.
 * <P>
 * This class exists so that we can register a StreamScope at runtime
 * with jobId info (lacking a Function level initialize(FuntionScope) mechanism)
 * and so the Console can differentiate a StreamScope peek from any other
 * random Peek oplet use.  Remove this oplet subclass if/when it's no
 * longer needed to achieve the above purpose.
 * </P>
 *
 * @param <T> Type of the tuple.
 */
public class StreamScope<T> extends Peek<T> {
  private static final long serialVersionUID = 1L;

  /**
   * Create a new instance.
   * @param streamScope the consumer function
   */
  public StreamScope(Consumer<T> streamScope) {
    super(streamScope);
  }

  @Override
  public void initialize(OpletContext<T, T> context) {
    super.initialize(context);
    registerStreamScope();
  }
  
  private void registerStreamScope() {
    StreamScopeRegistry rgy = getOpletContext().getService(StreamScopeRegistry.class);
    
    // see commentary in DevelopmentProvider.addStreamScopes()
    // re TODOs for being able to register for the "origin stream/oplet".
    
    String jobId = getOpletContext().getJobContext().getId();
    String opletId = getOpletContext().getId(); // TODO should be "origin oplet's" id
    int oport = 0;  // TODO should be the origin stream's oport index
    String streamId = StreamScopeRegistry.mkStreamId(jobId, opletId, oport);
    
    rgy.register(StreamScopeRegistry.nameForStreamId(streamId),
        (org.apache.edgent.streamscope.StreamScope<?>) getPeeker());

    // rgy.register(StreamScopeRegistry.nameForStreamAlias(alias), streamScope);
  }
    
}
