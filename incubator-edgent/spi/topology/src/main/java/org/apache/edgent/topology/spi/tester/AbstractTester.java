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
package org.apache.edgent.topology.spi.tester;

import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import org.apache.edgent.execution.Job;
import org.apache.edgent.execution.Submitter;
import org.apache.edgent.execution.Job.State;
import org.apache.edgent.topology.Topology;
import org.apache.edgent.topology.tester.Condition;
import org.apache.edgent.topology.tester.Tester;

import com.google.gson.JsonObject;

public abstract class AbstractTester implements Tester { 
    
    private Job job;

    @Override
    public boolean complete(Submitter<Topology, ? extends Job> submitter, JsonObject config, Condition<?> endCondition,
            long timeout, TimeUnit unit) throws Exception {

        long tmoMsec = Math.max(unit.toMillis(timeout), 1000);
        long maxTime = System.currentTimeMillis() + tmoMsec;

        Future<?> future = submitter.submit(topology(), config);
        // wait at most tmoMsec for the submit to create the job
        job = (Job) future.get(tmoMsec, TimeUnit.MILLISECONDS);

        // wait for the first of: endCondition, jobComplete, tmo
        while (!endCondition.valid()
                && getJob().getCurrentState() != State.CLOSED
                && System.currentTimeMillis() < maxTime) {
            Thread.sleep(100);
        }
        
        if (getJob().getCurrentState() != State.CLOSED)
            getJob().stateChange(Job.Action.CLOSE);

        return endCondition.valid();
    }
    
    @Override
    public Job getJob() {
        return job;
    }
    
    @Override
    public Condition<Boolean> and(final Condition<?>... conditions) {
        return new Condition<Boolean>() {

            @Override
            public boolean valid() {
                for (Condition<?> condition : conditions)
                    if (!condition.valid())
                        return false;
                return true;
            }

            @Override
            public Boolean getResult() {
                return valid();
            }
        };
    }
}
