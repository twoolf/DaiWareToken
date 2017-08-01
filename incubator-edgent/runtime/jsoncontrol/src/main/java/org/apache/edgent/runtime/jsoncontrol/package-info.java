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

/**
 * Control service that takes a Json message and invokes
 * an operation on a control service MBean.
 * 
 * <H3>Operations</H3>
 * A JSON object passed to {@link org.apache.edgent.runtime.jsoncontrol.JsonControlService#controlRequest(com.google.gson.JsonObject) controlRequest} with these name/value pairs is
 * handled as an operation resulting in a method call to a
 * void method on a control service MBean interface. 
 * <UL>
 * <LI>{@code type=}<em>type</em> - MBean type.</LI>
 * <LI>{@code alias=}<em>alias - Alias of the MBean.</em></LI>
 * <LI>{@code op=}<em>name</em> - Name of the operation to invoke, this is the method name on the MBean.</LI>
 * <LI>{@code arguments=}<em>optional list of arguments</em> - Arguments passed to the operation (method).</LI>
 * </UL>
 * The MBean must be uniquely identified through
 * its <em>type</em> and <em>alias</em> and previously registered using 
 * {@link org.apache.edgent.execution.services.ControlService#registerControl(String, String, String, Class, Object) registerControl()}.
 */

package org.apache.edgent.runtime.jsoncontrol;