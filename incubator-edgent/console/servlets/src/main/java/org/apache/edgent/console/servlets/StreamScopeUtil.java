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
package org.apache.edgent.console.servlets;

import java.lang.management.ManagementFactory;

import javax.management.JMX;
import javax.management.MBeanServer;
import javax.management.MalformedObjectNameException;
import javax.management.ObjectName;

import org.apache.edgent.streamscope.mbeans.StreamScopeMXBean;
import org.apache.edgent.streamscope.mbeans.StreamScopeRegistryMXBean;

public class StreamScopeUtil {
  
  /**
   * Get the StreamScope for the specified stream.
   * <P>
   * N.B. until certain runtime issues are worked out, the stream that a
   * StreamScopeMXBean is registered for is NOT the "origin stream" (opletId/oport)
   * that the StreamScope was created for.  Rather the registration is for
   * the actual StreamScope oplet's opletId and oport 0, so that's what must be
   * supplied as the parameters.
   * <BR>
   * See the commentary in StreamScope oplet code.
   * <BR>
   * Once that is addressed, opletId/oport of the "origin stream" will
   * need to be supplied as parameters.
   * </P>
   * 
   * @param jobId the job id (e.g., "JOB_0")
   * @param opletId the oplet id (e.g., "OP_2")
   * @param oport the oplet output port index (0-based)
   * 
   * @return null if no StreamScope registered for that stream.
   */
  public static StreamScopeMXBean getStreamScope(String jobId, String opletId, int oport) {
    return getRgy().lookup(jobId, opletId, oport);
  }

  private static StreamScopeRegistryMXBean getRgy() {
    MBeanServer mbs = ManagementFactory.getPlatformMBeanServer();
    ObjectName objName = mkObjectName(StreamScopeRegistryMXBean.class, StreamScopeRegistryMXBean.TYPE);
    return JMX.newMXBeanProxy(mbs, objName, StreamScopeRegistryMXBean.class);
  }
  
  private static ObjectName mkObjectName(Class<?> klass, String beanType) {
    StringBuffer sbuf = new StringBuffer();
    try {
      sbuf.append("*:interface=");
      sbuf.append(ObjectName.quote(klass.getCanonicalName()));
      sbuf.append(",type=");
      sbuf.append(ObjectName.quote(beanType));
      return new ObjectName(sbuf.toString());
    }
    catch (MalformedObjectNameException e) {
      
      // TODO logger.error("Unable to create ObjectName for "+sbuf, e);
      
      throw new RuntimeException("Unable to create ObjectName for "+sbuf, e);
    }
  }
  
}
