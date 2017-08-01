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
package org.apache.edgent.connectors.serial;


import java.util.concurrent.TimeUnit;

import org.apache.edgent.function.Consumer;
import org.apache.edgent.function.Function;
import org.apache.edgent.function.Supplier;
import org.apache.edgent.topology.TopologyElement;

/**
 * Access to a device (or devices) connected by a serial port.
 * A serial port at runtime is represented by
 * a {@link SerialPort}.
 * <P>
 * {@code SerialDevice} is typically used through
 * a protocol module that sends the appropriate bytes
 * to the port and decodes the bytes output by the port.
 * </P>
 * <P>
 * It is guaranteed that during any call to function returned by
 * this interface has exclusive access to {@link SerialPort}.
 * </P>
 */
public interface SerialDevice extends TopologyElement {
	
	/**
	 * Set the initialization function for this port.
	 * Can be used to send setup instructions to the
	 * device connected to this serial port.
	 * <BR>
	 * {@code initializer.accept(port)} is called once, passing a runtime
	 * {@link SerialPort} for this serial device.
	 * 
	 * @param initializer Function to be called when the application runs.
	 */
	void setInitializer(Consumer<SerialPort> initializer);
		
	/**
	 * Create a function that can be used to source a
	 * stream from a serial port device.
	 * <BR>
	 * Calling {@code get()} on the returned function will result in a call
	 * to {@code driver.apply(serialPort)}
	 * passing a runtime {@link SerialPort} for this serial device.
	 * The value returned by {@code driver.apply(serialPort)} is
	 * returned by this returned function.
	 * <BR>
	 * The function {@code driver} typically sends instructions to the
	 * serial port using {@link SerialPort#getOutput()} and then
	 * reads the result using {@link SerialPort#getInput()}.
	 * <P>
	 * Multiple instances of a supplier function can be created,
	 * for example to read different parameters from the
	 * device connected to the serial port. While each function
	 * is being called it has exclusive use of the serial port.
	 * </P>
	 * @param <T> Tuple type
	 * @param driver Function that interacts with the serial port to produce a value.
	 * @return Function that for each call will interact with the serial port to produce a value.
	 * 
	 * @see org.apache.edgent.topology.Topology#poll(Supplier, long, TimeUnit)
	 */	
	public <T> Supplier<T> getSource(Function<SerialPort,T> driver);
}
