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

import java.io.IOException;
import java.util.Map;

import javax.management.MalformedObjectNameException;
import javax.management.ObjectName;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;


public class ConsoleJobServlet extends HttpServlet {
    /**
	 * This servlet looks for any running jobs in the embedded environment in which the http server was started
	 */
	private static final long serialVersionUID = -2939472165693224428L;
	private static final Logger logger = LoggerFactory.getLogger(ConsoleJobServlet.class);

	@Override
    public void doGet(HttpServletRequest request, HttpServletResponse response)
        throws ServletException, IOException {
        // jobsInfo to return just the job id, etc
        // jobgraph to return the graph of the job + jobId
        Map<String,String[]> parameterMap = request.getParameterMap();
        String jobId = "";
        boolean jobsInfo = false;
        boolean jobGraph = false;
        for(Map.Entry<String,String[]> entry : parameterMap.entrySet()) {
                if (entry.getKey().equals("jobsInfo")) {
                        String[] vals = entry.getValue();
                        if (vals[0].equals("true")) {
                                jobsInfo = true;
                        }
                } else if (entry.getKey().equals("jobgraph")) {
                        String[] vals = entry.getValue();
                        if (vals[0].equals("true")) {
                                jobGraph = true;
                        }
                } else if (entry.getKey().equals("jobId")) {
                        String[] ids = entry.getValue();
                        if (ids.length == 1) {
                                jobId = ids[0];
                        }
                }
        }
        

        StringBuffer sbuf = new StringBuffer();
        sbuf.append("*:interface=");
        sbuf.append(ObjectName.quote("org.apache.edgent.execution.mbeans.JobMXBean"));
        sbuf.append(",type=");
        sbuf.append(ObjectName.quote("job"));
        
        if (!jobId.equals("")) {
        	sbuf.append(",id=");
        	sbuf.append(ObjectName.quote(jobId));
        } 
        sbuf.append(",*");
        
        ObjectName jobObjName = null;
        try {
        	jobObjName = new ObjectName(sbuf.toString());
        	} catch (MalformedObjectNameException e) {
        	    logger.error("Exception caught while initializing ObjectName", e);
        	}
        String jsonString = "";
        if (jobsInfo) {
        	jsonString = JobUtil.getJobsInfo(jobObjName);
        } else if (jobGraph && !(jobId.equals("")) && !(jobId.equals("undefined"))) {
            jsonString = JobUtil.getJobGraph(jobObjName);
        }

        response.setContentType("application/json");
        response.setCharacterEncoding("UTF-8");
        response.getWriter().write(jsonString);
        
	}		
}
