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
package org.apache.edgent.runtime.etiao.graph;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

import org.apache.edgent.graph.Connector;
import org.apache.edgent.graph.Vertex;
import org.apache.edgent.graph.spi.DirectEdge;
import org.apache.edgent.oplet.Oplet;
import org.apache.edgent.oplet.core.FanOut;
import org.apache.edgent.oplet.core.Peek;

class EtiaoConnector<P> implements Connector<P> {

  /** The connector's associated vertex and its output port index. */
  private final ExecutableVertex<?, ?, P> vertex;
  private final int oport;

  /*
   * The connector's shared state.
   * 
   * All of the connectors for a single logical stream share the same state.
   * i.e., the connectors for the internally created peek and fanout vertices
   * share their associated primary ("the stream's") connector's state.
   */
  private SharedState<P> state;

  /**
   * Shared connector state.
   * 
   * @param <P> The Input/Output port type.
   */
  private static class SharedState<P> {    
    String alias;
    private Set<String> tags = new HashSet<>();
    
    /** the primary ("the stream's") connector */
    EtiaoConnector<P> primaryConnector;
    
    /** where to add the next peek and 1st connect() op */
    EtiaoConnector<P> activeConnector;
    
    /** the connect() added non-peek or FanOut target */
    Target<P> connectTarget;
    
    /** FanOut when the connector has >1 connect() added targets */
    ExecutableVertex<FanOut<P>, P, P> fanOutVertex;

    public SharedState(EtiaoConnector<P> connector) {
      primaryConnector = connector;
      activeConnector = connector;
    }

    public String toString() {
      return "{" 
          // avoid activeConnector.toString() recursion
          + " activeConnector=<" + activeConnector.oport + "," + activeConnector.vertex + ">"
          + " primaryConnector=<" + primaryConnector.oport + "," + primaryConnector.vertex + ">"
          + " connectTarget=" + connectTarget
          + " fanOutVertex=" + fanOutVertex
          + " alias=" + alias
          + " tags=" + tags
          + "}";
    }
  }

  /**
   * Create a new connector.
   * 
   * @param vertex the connector's associated Vertex
   * @param oport the connector's output port index
   */
  public EtiaoConnector(ExecutableVertex<?, ?, P> vertex, int oport) {
    this.vertex = vertex;
    this.oport = oport;
    this.state = new SharedState<P>(this); // later reset for "internal" connectors
  }

  @Override
  public DirectGraph graph() {
    return vertex.graph();
  }

  @Override
  public boolean isConnected() {
    // see Connector.isConnected() doc for semantics
    // the primary connector can return true.  
    // internal peek connectors must return false (peekAll() depends on it)
    // internal fanout connectors return false (used to be true and peekAll()
    //    callers had to explicitly exclude fanout ops).
   
    return state.connectTarget != null && this == state.primaryConnector;
  }

  private boolean isFanOut() {
    return state.fanOutVertex != null;
  }

  @Override
  public void connect(Vertex<?, P, ?> target, int targetPort) {
    // to be used only for connecting to non-internal (non peek/fanout) vertex
    
    if (!isConnected()) {  // 1st connect()
      state.connectTarget = connectActiveDirect(new Target<P>((ExecutableVertex<?, P, ?>) target, targetPort));
      return;
    }

    if (!isFanOut()) {  // 2nd connect()
      // Add a FanOut oplet, initially with a single output port
      // connected to 1st connect()'s target
      state.fanOutVertex = newInternalVertex(new FanOut<P>(), 1, 1);
      EtiaoConnector<P> fanOutConnector = state.fanOutVertex.getConnectors().get(0);
      fanOutConnector.takeTarget();

      // Connect to the FanOut oplet. The FanOut becomes the connectTarget.
      state.connectTarget = connectActiveDirect(new Target<P>(state.fanOutVertex, 0));
    }
    
    // 2nd-nth connect(): add target as another connector/connection from the FanOut
    EtiaoConnector<P> fanOutConnector = state.fanOutVertex.addOutput();
    fanOutConnector.state = state;
    fanOutConnector.connectDirect(new Target<P>((ExecutableVertex<?, P, ?>) target, targetPort));
    
    assert isConnected();
  }

  @Override
  public <N extends Peek<P>> Connector<P> peek(N oplet) {
    // see Connector.peek() method/class doc for the semantics
    
    ExecutableVertex<N, P, P> peekVertex = newInternalVertex(oplet, 1, 1);
    EtiaoConnector<P> peekConnector = peekVertex.getConnectors().get(0);

    // The peek takes over the connection to connect() added target(s).
    if (isConnected()) {
      peekConnector.takeTarget();
    }

    // Connect to the new peek.  It becomes the activeConnector / new addition point.
    connectActiveDirect(new Target<P>(peekVertex, 0));
    state.activeConnector = peekConnector;

    return this;
  }

  /**
   * Create a new vertex for internal use (with shared connector state)
   * @param op a Peek or FanOut oplet
   * @param nInputs number of input ports
   * @param nOutputs number of output ports
   * @return the new vertex
   */
  private <N extends Oplet<P, P>> ExecutableVertex<N, P, P> 
  newInternalVertex(N op, int nInputs, int nOutputs) {
    ExecutableVertex<N, P, P> vertex = graph().insert(op, nInputs, nOutputs);
    for (EtiaoConnector<P> connector : vertex.getConnectors()) {
      connector.state = state;
    }
    return vertex;
  }

  /**
   * Disconnect the connect() added target(s)
   * @return the target
   */
  private Target<P> disconnectTarget() {
    assert state.connectTarget != null;

    state.activeConnector.vertex.disconnect(state.activeConnector.oport);
    Target<P> target = state.connectTarget;
    state.connectTarget = null;
    assert state.connectTarget == null;

    return target;
  }

  /**
   * Take activeConnector's connection to the connect() added target(s).
   */
  private void takeTarget() {
    state.connectTarget = connectDirect(disconnectTarget());
  }

  /**
   * Connect this to the target
   * @param target
   * @return the target parameter
   */
  private Target<P> connectDirect(Target<P> target) {
    vertex.connect(oport, target,
        new DirectEdge(this, vertex, oport, target.vertex, target.port));
    return target;
  }

  /**
   * Connect the activeConnector to the target
   * @param target
   * @return the target parameter
   */
  private Target<P> connectActiveDirect(Target<P> target) {
    ExecutableVertex<?,?,P> vertex = state.activeConnector.vertex;
    int oport = state.activeConnector.oport;
    vertex.connect(oport, target, 
        new DirectEdge(state.activeConnector,
            vertex, oport, target.vertex, target.port));
    return target;
  }

  @Override
  public void tag(String... values) {
    for (String v : values)
      state.tags.add(v);
  }

  @Override
  public Set<String> getTags() {
    return Collections.unmodifiableSet(state.tags);
  }

  @Override
  public void alias(String alias) {
    if (state.alias != null)
      throw new IllegalStateException("alias already set");
    state.alias = alias;
  }

  @Override
  public String getAlias() {
    return state.alias;
  }

  /**
   * Intended only as a debug aid and content is not guaranteed.
   */
  @Override
  public String toString() {
    return "{"
        + getClass().getSimpleName()
        + " oport=" + oport
        + " vertex=" + vertex
        + " state=" + state 
        + "}";
  }

}
