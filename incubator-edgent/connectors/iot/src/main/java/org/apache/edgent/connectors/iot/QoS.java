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
package org.apache.edgent.connectors.iot;

/**
 * Device event quality of service levels.
 * The QoS levels match the MQTT specification.
 * <BR>
 * An implementation of {@link IotDevice} may not
 * support all QoS levels.
 * 
 * @see <a href="http://mqtt.org/">mqtt.org</a>
 * @see IotDevice#events(org.apache.edgent.topology.TStream, String, int)
 * @see IotDevice#events(org.apache.edgent.topology.TStream, org.apache.edgent.function.Function, org.apache.edgent.function.UnaryOperator, org.apache.edgent.function.Function)

 */
public interface QoS {
    
    /**
     * The message containing the event arrives at the message hub either once or not at all.
     * <BR>
     * Value is {@code 0}.
     */
    Integer AT_MOST_ONCE = 0;
    
    /**
     * Fire and forget the event. Synonym for {@link #AT_MOST_ONCE}.
     * <BR>
     * Value is {@code 0}.
     */
    Integer FIRE_AND_FORGET = 0;
    
    /**
     * The message containing the event arrives at the message hub at least once.
     * The message may be seen at the hub multiple times.
     * <BR>
     * Value is {@code 1}.
     */
    Integer AT_LEAST_ONCE = 1;
    
    /**
     * The message containing the event arrives at the message hub exactly once.
     * <BR>
     * Value is {@code 2}.
     */ 
    Integer EXACTLY_ONCE = 2;
}
