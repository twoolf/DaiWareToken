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
import java.util.Iterator;
import java.util.Map;

import javax.management.ObjectInstance;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.google.gson.Gson;

public class ConsoleMetricsServlet extends HttpServlet {

	/*
	 * This servlet can accept requests for all jobs or a single job, but will
	 * most likely be rewritten to only accept a single job as a parameter
	 */
	private static final long serialVersionUID = -1548438576311809996L;
	
	public void doGet(HttpServletRequest request, HttpServletResponse response) 
		throws ServletException, IOException {
		// get the parameters for the job and operator (?) to fetch the metrics for
		
		Map<String,String[]> parameterMap = request.getParameterMap();
		String[] jobIds;
		String jobId = "";
		String[] metricNames;
		String metricName = "";
		boolean availableMetrics = false;
		boolean getAllMetrics = false;
		for(Map.Entry<String,String[]> entry : parameterMap.entrySet()) {
			if (entry.getKey().equals("job")) {
				jobIds = entry.getValue();
				if (jobIds.length == 1) {
					jobId = jobIds[0];
				}
			} else if (entry.getKey().equals("metric")) {
				metricNames = entry.getValue();
				if (metricNames.length == 1) {
					metricName = metricNames[0];
				}
			} else if (entry.getKey().equals("availableMetrics")) {
				String[] getMetrics = entry.getValue();
				if (getMetrics.length == 1) {
					availableMetrics = true;
				}
			} else if (entry.getKey().equals("getAllMetrics")) {
				String[] getAll = entry.getValue();
				if (getAll.length == 1) {
					getAllMetrics = true;
				}
			}
		}

		if (!jobId.equals("")) {
			Iterator<ObjectInstance> meterIterator = MetricsUtil.getMeterObjectIterator(jobId);
			Iterator<ObjectInstance> counterIterator = MetricsUtil.getCounterObjectIterator(jobId);
			if (availableMetrics) {
				MetricsGson gsonJob = MetricsUtil.getAvailableMetricsForJob(jobId, meterIterator, counterIterator);
				Gson gson = new Gson();
		    	response.setContentType("application/json");
		    	response.setCharacterEncoding("UTF-8");
		    	response.getWriter().write(gson.toJson(gsonJob));
				return;
			}
			
			if (getAllMetrics) {
				MetricsGson gsonJob = MetricsUtil.getAllRateMetrics(jobId);
				Gson gson = new Gson();
		    	response.setContentType("application/json");
		    	response.setCharacterEncoding("UTF-8");
		    	response.getWriter().write(gson.toJson(gsonJob));
				return;
			}
			
			MetricsGson mGson = MetricsUtil.getMetric(jobId, metricName, meterIterator, counterIterator);
			String jsonString = "";

			Gson gson = new Gson();
			jsonString = gson.toJson(mGson);
	    	response.setContentType("application/json");
	    	response.setCharacterEncoding("UTF-8");
	    	
	    	response.getWriter().write(jsonString);
		}
	}

}
