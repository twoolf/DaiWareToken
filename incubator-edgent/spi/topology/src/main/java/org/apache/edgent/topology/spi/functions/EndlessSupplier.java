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
package org.apache.edgent.topology.spi.functions;

import java.util.Iterator;

import org.apache.edgent.function.Functions;
import org.apache.edgent.function.Supplier;
import org.apache.edgent.function.WrappedFunction;

public class EndlessSupplier<T> extends WrappedFunction<Supplier<T>> implements Supplier<Iterable<T>> {
    /**
     * 
     */
    private static final long serialVersionUID = 1L;

    public EndlessSupplier(Supplier<T> data) {
        super(Functions.synchronizedSupplier(data));
    }

    @Override
    public final Iterable<T> get() {
        return new Iterable<T>() {

            @Override
            public Iterator<T> iterator() {

                return new Iterator<T>() {
                    @Override
                    public boolean hasNext() {
                        return true;
                    }

                    @Override
                    public T next() {
                        return f().get();
                    }

                    @Override
                    public void remove() {
                        throw new UnsupportedOperationException();
                    }
                };

            }

        };
    }
}
