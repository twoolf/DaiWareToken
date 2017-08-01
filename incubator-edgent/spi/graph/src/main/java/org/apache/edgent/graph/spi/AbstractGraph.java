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

import java.util.ArrayList;
import java.util.List;

import org.apache.edgent.function.Predicate;
import org.apache.edgent.function.Supplier;
import org.apache.edgent.graph.Connector;
import org.apache.edgent.graph.Graph;
import org.apache.edgent.graph.Vertex;
import org.apache.edgent.oplet.Oplet;
import org.apache.edgent.oplet.core.Peek;
import org.apache.edgent.oplet.core.Source;

/**
 * A skeletal implementation of the {@link Graph} interface,
 * to minimize the effort required to implement the interface.
 * 
 * @param <G> unnecessary?
 */
public abstract class AbstractGraph<G> implements Graph {
 
    @Override
    public <N extends Source<P>, P> Connector<P> source(N oplet) {
        return insert(oplet, 0, 1).getConnectors().get(0);
    }

    @Override
    public <N extends Oplet<C, P>, C, P> Connector<P> pipe(Connector<C> output, N oplet) {
        Vertex<N, C, P> pipeVertex = insert(oplet, 1, 1);
        output.connect(pipeVertex, 0);

        return pipeVertex.getConnectors().get(0);
    }

    @SuppressWarnings({ "unchecked", "rawtypes" })
    @Override
    public void peekAll(Supplier<? extends Peek<?>> supplier, Predicate<Vertex<?, ?, ?>> select) {
        // Select vertices which satisfy the specified predicate
        List<Vertex<?, ?, ?>> vertices = new ArrayList<>();
        for (Vertex<?, ?, ?> v : getVertices()) {
            if (select.test(v)) {
                vertices.add(v);
            }
        }
        // Insert peek oplets on isConnected() output ports
        for (Vertex<?, ?, ?> v : vertices) {
            List<? extends Connector<?>> connectors = v.getConnectors();
            for (Connector<?> c : connectors) {
                if (c.isConnected()) {
                    Peek<?> oplet = supplier.get();
                    c.peek((Peek) oplet);
                }
            }
        }
    }
}
