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
package org.apache.edgent.runtime.etiao.graph.model;

import java.util.HashSet;
import java.util.Set;

import org.apache.edgent.graph.Edge;

/**
 * Represents an edge between two {@link VertexType} nodes.
 */
public class EdgeType {
    /** Source vertex identifier */
	private final String sourceId;
    /** Source output port index */
	private final int sourceOutputPort;
    /** Target vertex identifier */
	private final String targetId;
    /** Target input port index */
	private final int targetInputPort;
    /** Set of tags associated with this edge */
    private final Set<String> tags;
    /** Edge's alias.  Null if none. */
    private final String alias;
    
    public EdgeType() {
        this.sourceId = null;
        this.sourceOutputPort = 0;
        this.targetId = null;
        this.targetInputPort = 0;
        this.tags = new HashSet<>();
        this.alias = null;
    }

    public EdgeType(Edge value, IdMapper<String> ids) {
        this.sourceId = ids.getId(value.getSource()).toString();
        this.sourceOutputPort = value.getSourceOutputPort();
        this.targetId = ids.getId(value.getTarget()).toString();
        this.targetInputPort = value.getTargetInputPort();
        this.tags = value.getTags();
        this.alias = value.getAlias();
    }

    public String getSourceId() {
        return sourceId;
    }
    
    public int getSourceOutputPort() {
        return sourceOutputPort;
    }
    
    public String getTargetId() {
        return targetId;
    }
    
    public int getTargetInputPort() {
        return targetInputPort;
    }

    public Set<String> getTags() {
        return tags;
    }

    public String getAlias() {
        return alias;
    }
}
