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

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.edgent.function.Consumer;
import org.apache.edgent.graph.Edge;
import org.apache.edgent.graph.spi.AbstractVertex;
import org.apache.edgent.graph.spi.DirectEdge;
import org.apache.edgent.oplet.Oplet;
import org.apache.edgent.oplet.OutputPortContext;
import org.apache.edgent.runtime.etiao.Invocation;

public class ExecutableVertex<N extends Oplet<C, P>, C, P> extends AbstractVertex<N, C, P> {

    private static final Edge DISCONNECTED = new DirectEdge();
    private DirectGraph graph;
    private final Invocation<N, C, P> invocation;
    private final List<EtiaoConnector<P>> connectors;
    private final List<Edge> edges;
    
    ExecutableVertex(DirectGraph graph, Invocation<N, C, P> invocation) {
        this.graph = graph;
        this.invocation = invocation;
        connectors = new ArrayList<>(invocation.getOutputCount());
        for (int i = 0; i < invocation.getOutputCount(); i++) {
            addConnector(i);
        }
        edges = new ArrayList<>(invocation.getOutputCount());
        for (int i = 0; i < invocation.getOutputCount(); i++) {
            edges.add(DISCONNECTED);
        }
    }

    private EtiaoConnector<P> addConnector(int index) {
		EtiaoConnector<P> connector = new EtiaoConnector<>(this, index);
        connectors.add(connector);  
        return connector;
    }

    @Override
    public DirectGraph graph() {
        return graph;
    }

    @Override
    public N getInstance() {
        return invocation.getOplet();
    }

    @Override
    public EtiaoConnector<P> addOutput() {
        int outputPort = invocation.addOutput();
        int edgeIndex = addEdge();

        assert outputPort == edgeIndex;

        return addConnector(outputPort);
    }


    @Override
    public List<EtiaoConnector<P>> getConnectors() {
        return Collections.unmodifiableList(connectors);
    }

    public String getInvocationId() {
        return invocation.getId();
    }

	void disconnect(int sourcePort) {
		invocation.disconnect(sourcePort);
		edges.set(sourcePort, DISCONNECTED);
	}

	/**
	 * Connect this Vertex's source port to a target Vertex input port
	 * using the given edge.
	 * 
	 * @param sourcePort
	 * @param target
	 * @param edge
	 */
    void connect(int sourcePort, Target<P> target, Edge edge) {
        if (edge == null)
            throw new NullPointerException();
        Consumer<P> input = target.vertex.invocation.getInputs().get(target.port);
        invocation.setTarget(sourcePort, input);
        edges.set(sourcePort, edge);
        invocation.setContext(sourcePort, new MyOutputContext(connectors.get(sourcePort)));
    }
    
    private static class MyOutputContext implements OutputPortContext {
        private final String alias;
        MyOutputContext(EtiaoConnector<?> connector) {
            alias = connector.getAlias();
        }
        @Override
        public String getAlias() {
            return alias;
        }
    }

    int addEdge() {
        int index = edges.size();
        edges.add(DISCONNECTED);
        return index;
    }

    List<Edge> getEdges() {
        List<Edge> connectedEdges = new ArrayList<>();
        for (Edge de : edges) {
            if (de != DISCONNECTED)
                connectedEdges.add(de);
        }
        return Collections.unmodifiableList(connectedEdges);
    }
    
    /** For debug. Contents subject to change. */
    @Override
    public String toString() {
      return "{" 
          + "invocation=" + invocation
          // + " edges=" + edges
          + "}";
    }
}
