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

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.apache.edgent.execution.services.RuntimeServices;
import org.apache.edgent.function.Consumer;
import org.apache.edgent.function.Functions;
import org.apache.edgent.oplet.JobContext;
import org.apache.edgent.oplet.Oplet;
import org.apache.edgent.oplet.OutputPortContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * An {@link Oplet} invocation in the context of the 
 * <a href="{@docRoot}/org/apache/edgent/runtime/etiao/package-summary.html">ETIAO</a> runtime.  
 *
 * @param <T> 
 *            Oplet type.
 * @param <I>
 *            Data container type for input tuples.
 * @param <O>
 *            Data container type for output tuples.
 */
public class Invocation<T extends Oplet<I, O>, I, O> implements AutoCloseable {
    /** Prefix used by oplet unique identifiers. */
    public static final String ID_PREFIX = "OP_";
    private static final OutputPortContext DEFAULT_OUTPUT_CONTEXT = 
        new OutputPortContext() {
            @Override
            public String getAlias() {
                return null;
            }
        };

   /**
    * Runtime unique identifier.
    */
    private final String id; 
    private T oplet;

    private List<Consumer<O>> outputs;
    private List<SettableForwarder<I>> inputs;
    private List<OutputPortContext> outputContext;
    private static final Logger logger = LoggerFactory.getLogger(Invocation.class);

    protected Invocation(String id, T oplet, int inputCount, int outputCount) {
    	this.id = id;
        this.oplet = oplet;
        inputs = inputCount == 0 ? Collections.emptyList() : new ArrayList<>(inputCount);
        for (int i = 0; i < inputCount; i++) {
            inputs.add(new SettableForwarder<>());
        }

        outputs = outputCount == 0 ? Collections.emptyList() : new ArrayList<>(outputCount);
        outputContext = outputCount == 0 ? Collections.emptyList() : new ArrayList<>(outputCount);
        for (int i = 0; i < outputCount; i++) {
            addOutput();
        }
    }

    /**
     * Returns the unique identifier associated with this {@code Invocation}.
     * 
     * @return unique identifier
     */
    public String getId() {
        return id;
    }
    
    /**
     * Returns the oplet associated with this {@code Invocation}.
     * 
     * @return the oplet associated with this invocation
     */
    public T getOplet() {
        return oplet;
    }

    /**
     * Returns the number of outputs for this invocation.
     * @return the number of outputs
     */
    public int getOutputCount() {
        return outputs.size();
    }
    
    /**
     * Adds a new output.  By default, the output is connected to a Consumer 
     * that discards all items passed to it.
     * 
     * @return the index of the new output
     */
    public int addOutput() {
        int index = outputs.size();
        outputs.add(Functions.discard());
        outputContext.add(DEFAULT_OUTPUT_CONTEXT);
        return index;
    }

    /**
     * Disconnects the specified port by connecting to a no-op {@code Consumer} implementation.
     * 
     * @param port the port index
     */
    public void disconnect(int port) {
        outputs.set(port, Functions.discard());
    }

    /**
     * Disconnects the specified port and reconnects it to the specified target.
     * 
     * @param port index of the port which is reconnected
     * @param target target the port gets connected to
     */
    public void setTarget(int port, Consumer<O> target) {
        disconnect(port);
        outputs.set(port, target);
    }

    /**
     * Set the specified output port's context.
     * 
     * @param port index of the output port
     * @param context the new {@link OutputPortContext}
     */
    public void setContext(int port, OutputPortContext context) {
        if (context == null)
            throw new NullPointerException();
        outputContext.set(port, context);
    }

    /**
     * Returns the list of input stream forwarders for this invocation.
     * @return the list
     */
    public List<? extends Consumer<I>> getInputs() {
        return inputs;
    }

    /**
     * Initialize the invocation.
     * 
     * @param job the context of the current job
     * @param services service provider for this invocation
     */
    public void initialize(JobContext job, RuntimeServices services) {

        InvocationContext<I, O> context = new InvocationContext<I, O>(
        		id, job, services, 
                inputs.size(),
                outputs, outputContext);

        try {
            oplet.initialize(context);
        } catch (Exception e) {
            logger.error("Error while initializing oplet", e);
        }
        List<? extends Consumer<I>> streamers = oplet.getInputs();
        for (int i = 0; i < inputs.size(); i++)
            inputs.get(i).setDestination(streamers.get(i));
    }

    /**
     * Start the oplet. Oplets must not submit any tuples not derived from
     * input tuples until this method is called.
     */
    public void start() {
        oplet.start();
    }

    @Override
    public void close() throws Exception {
        oplet.close();
    }
    
    /** For debug. Contents subject to change. */
    @Override
    public String toString() {
      return "{"
          + "id=" + getId()
          + " oplet=" + oplet.getClass().getSimpleName()
          + "}";
    }
}
