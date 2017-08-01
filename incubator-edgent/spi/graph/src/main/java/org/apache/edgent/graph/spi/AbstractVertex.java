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

import org.apache.edgent.graph.Vertex;
import org.apache.edgent.oplet.Oplet;

/**
 * Placeholder for a skeletal implementation of the {@link Vertex} interface,
 * to minimize the effort required to implement the interface.
 *
 * @param <OP>
 *            Oplet type associated with this vertex.
 * @param <I>
 *            Data container type for input tuples.
 * @param <O>
 *            Data container type for output tuples.
 */
public abstract class AbstractVertex<OP extends Oplet<I, O>, I, O> implements Vertex<OP, I, O> {
}
