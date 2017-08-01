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
package org.apache.edgent.graph.spi;

import java.util.Set;

import org.apache.edgent.graph.Connector;
import org.apache.edgent.graph.Edge;
import org.apache.edgent.graph.Vertex;

/**
 * This class provides a simple implementation of the {@link Edge} interface in
 * the context of a {@code DirectProvider}.
 */
public class DirectEdge implements Edge {
	
    private final Connector<?> connector;
	private final Vertex<?, ?, ?> source;
	private final int sourcePort;
	
	private final Vertex<?, ?, ?> target;
	private final int targetPort;
	
	public DirectEdge(
			Connector<?> connector,
			Vertex<?, ?, ?> source, int sourcePort,
			Vertex<?, ?, ?> target, int targetPort) {
	    this.connector = connector;
		this.source = source;
		this.sourcePort = sourcePort;
		this.target = target;
		this.targetPort = targetPort;
	}

	/**
	 * Create disconnected edge.
	 */
	public DirectEdge() {
        this.connector = null;
        this.source = null;
        this.sourcePort = 0;
        this.target = null;
        this.targetPort = 0;
    }

    @Override
	public Vertex<?, ?, ?> getSource() {
		return source;
	}

	@Override
	public int getSourceOutputPort() {
		return sourcePort;
	}

	@Override
	public Vertex<?, ?, ?> getTarget() {
		return target;
	}

	@Override
	public int getTargetInputPort() {
		return targetPort;
	}

    @Override
    public Set<String> getTags() {
        return connector.getTags();
    }

    @Override
    public String getAlias() {
        return connector.getAlias();
    }
    
//    /** For debug. Contents subject to change. */
//    @Override
//    public String toString() {
//      return "{"
//          + "source=" + (source==null ? "null" : source.getInstance())
//          + " sourcePort=" + sourcePort
//          + " target=" + (target==null ? "null" : target.getInstance())
//          + " targetPort=" + targetPort
//          + "}";
//    }
}
