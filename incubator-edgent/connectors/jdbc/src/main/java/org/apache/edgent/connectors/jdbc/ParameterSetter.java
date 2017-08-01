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
package org.apache.edgent.connectors.jdbc;

import java.sql.PreparedStatement;
import java.sql.SQLException;

/**
 * Function that sets parameters in a JDBC SQL {@link java.sql.PreparedStatement}.
 *
 * @param <T> stream tuple type
 */
@FunctionalInterface
public interface ParameterSetter<T> {
    /**
     * Set 0 or more parameters in a JDBC PreparedStatement. 
     * <p>
     * Sample use for a PreparedStatement of:
     * <br>
     * {@code "SELECT id, firstname, lastname FROM persons WHERE id = ?"}
     * <pre>{@code
     * ParameterSetter<PersonId> ps = (personId,stmt) -> stmt.setInt(1, personId.getId());
     * }</pre>
     *
     * @param t stream tuple of type T
     * @param stmt PreparedStatement
     * @throws SQLException on failure
     */
    void setParameters(T t, PreparedStatement stmt) throws SQLException;
}
