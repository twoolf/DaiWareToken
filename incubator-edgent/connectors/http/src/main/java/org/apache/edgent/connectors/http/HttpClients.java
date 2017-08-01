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

package org.apache.edgent.connectors.http;

import org.apache.edgent.function.Supplier;
import org.apache.http.auth.AuthScope;
import org.apache.http.auth.UsernamePasswordCredentials;
import org.apache.http.client.CredentialsProvider;
import org.apache.http.impl.client.BasicCredentialsProvider;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClientBuilder;

/**
 * Creation of HTTP Clients.
 * 
 * This methods are called at runtime to create
 * HTTP clients for {@link HttpStreams}. They are
 * passed into methods such as
 * {@link HttpStreams#requests(org.apache.edgent.topology.TStream, Supplier, org.apache.edgent.function.Function, org.apache.edgent.function.Function, org.apache.edgent.function.BiFunction)}
 * as functions, for example:
 * <UL style="list-style-type:none">
 * <LI>{@code () -> HttpClients::noAuthentication } // using a method reference</LI>
 *  <LI>{@code () -> HttpClients.basic("user", "password") } // using a lambda expression</LI>
 * </UL>
 *
 * @see HttpStreams
 */
public class HttpClients {
    
    /**
     * Create HTTP client with no authentication.
     * @return HTTP client with basic authentication.
     * 
     * @see HttpStreams
     */
    public static CloseableHttpClient noAuthentication() {
        return HttpClientBuilder.create().build();
    }
    
    /**
     * Create a basic authentication HTTP client with a fixed user and password.
     * @param user User for authentication
     * @param password Password for authentication
     * @return HTTP client with basic authentication.
     * 
     * @see HttpStreams
     */
    public static CloseableHttpClient basic(String user, String password) {
        return basic(()->user, ()->password);
    }
    
    /**
     * Method to create a basic authentication HTTP client.
     * The functions {@code user} and {@code password} are called
     * when this method is invoked to obtain the user and password
     * and runtime.
     * 
     * @param user Function that provides user for authentication
     * @param password  Function that provides password for authentication
     * @return HTTP client with basic authentication.
     * 
     * @see HttpStreams
     */
    public static CloseableHttpClient basic(Supplier<String> user, Supplier<String> password) {

        CredentialsProvider credsProvider = new BasicCredentialsProvider();
        credsProvider.setCredentials(AuthScope.ANY,
                new UsernamePasswordCredentials(user.get(), password.get()));
 
        return HttpClientBuilder.create().setDefaultCredentialsProvider(credsProvider).build();
    }
}
