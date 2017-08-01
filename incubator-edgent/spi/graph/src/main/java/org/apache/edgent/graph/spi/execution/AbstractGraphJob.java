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
package org.apache.edgent.graph.spi.execution;

import org.apache.edgent.execution.Job;

/**
 * Placeholder for a skeletal implementation of the {@link Job} interface,
 * to minimize the effort required to implement the interface.
 */
public abstract class AbstractGraphJob implements Job {
    private State currentState;
    private State nextState;
    private Health health;
    private String lastError;

    protected AbstractGraphJob() {
        this.currentState = State.CONSTRUCTED;
        this.nextState = currentState;
        this.health = Health.HEALTHY;
        this.lastError = new String();
    }

    @Override
    public synchronized State getCurrentState() {
        return currentState;
    }

    @Override
    public synchronized State getNextState() {
        return nextState;
    }

    @Override
    public abstract void stateChange(Action action);
    
    @Override
    public Health getHealth() {
        return health;
    }

    @Override
    public String getLastError() {
        return lastError;
    }

    protected synchronized boolean inTransition() {
        return getNextState() != getCurrentState();
    }

    protected synchronized void setNextState(State value) {
        this.nextState = value;
    }
    
    protected synchronized void completeTransition() {
        if (inTransition()) {
            currentState = nextState;
        }
    }
    
    protected void setHealth(Health value) {
        this.health = value;
    }
    
    protected void setLastError(String value) {
        this.lastError = value;
    }
}
