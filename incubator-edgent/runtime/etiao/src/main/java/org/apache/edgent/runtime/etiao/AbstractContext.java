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
package org.apache.edgent.runtime.etiao;

import org.apache.edgent.execution.services.RuntimeServices;
import org.apache.edgent.oplet.JobContext;
import org.apache.edgent.oplet.OpletContext;

/**
 * Provides a skeletal implementation of the {@link OpletContext}
 * interface.
 * 
 * @param <I> Tuple type of input streams
 * @param <O> Tuple type of output streams
 */
public abstract class AbstractContext<I, O> implements OpletContext<I, O> {

    private final JobContext job;
    private final RuntimeServices services;

    public AbstractContext(JobContext job, RuntimeServices services) {
        this.job = job;
        this.services = services;
    }
    
    @Override
    public <T> T getService(Class<T> serviceClass) {
        return services.getService(serviceClass);
    }
    
    @Override
    public JobContext getJobContext() {
        return job;
    }
    /**
     * Creates a unique name within the context of the current runtime.
     * <p>
     * The default implementation adds a suffix composed of the package 
     * name of this interface, the current job and oplet identifiers, 
     * all separated by periods ({@code '.'}).  Developers should use this 
     * method to avoid name clashes when they store or register the name in 
     * an external container or registry.
     *
     * @param name name (possibly non-unique)
     * @return unique name within the context of the current runtime.
     */
    @Override
    public String uniquify(String name) {
        return new StringBuilder(name).
                append('.').append(OpletContext.class.getPackage().getName()).
                append('.').append(getJobContext().getId()).
                append('.').append(getId()).toString();
    }
}
