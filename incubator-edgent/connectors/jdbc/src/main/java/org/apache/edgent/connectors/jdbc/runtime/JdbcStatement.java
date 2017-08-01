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
package org.apache.edgent.connectors.jdbc.runtime;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

import org.apache.edgent.connectors.jdbc.ParameterSetter;
import org.apache.edgent.connectors.jdbc.ResultsHandler;
import org.apache.edgent.connectors.jdbc.StatementSupplier;
import org.apache.edgent.function.Consumer;
import org.apache.edgent.function.Function;
import org.slf4j.Logger;

public class JdbcStatement<T,R> implements Function<T,Iterable<R>>,Consumer<T>,AutoCloseable {
    private static final long serialVersionUID = 1L;
    private final Logger logger;
    private final JdbcConnector connector;
    private final StatementSupplier stmtSupplier;
    private final ParameterSetter<T> paramSetter;
    private final ResultsHandler<T,R> resultsHandler;
    private PreparedStatement stmt;
    private long nTuples;
    private long nTuplesFailed;
    
    private void closeStmt() {
        if (stmt != null) {
            logger.trace("closing statement");
            PreparedStatement tmp = stmt;
            stmt = null;
            try {
                tmp.close();
            }
            catch (SQLException e) {
                logger.error("close stmt failed", e);
            }
        }
    }
    
    public JdbcStatement(JdbcConnector connector,
            StatementSupplier stmtSupplier, ParameterSetter<T> paramSetter,
                ResultsHandler<T,R> resultsHandler) {
        this.logger = connector.getLogger();
        this.connector = connector;
        this.stmtSupplier = stmtSupplier;
        this.paramSetter = paramSetter;
        this.resultsHandler = resultsHandler;
    }
    
    public JdbcStatement(JdbcConnector connector,
            StatementSupplier stmtSupplier, ParameterSetter<T> paramSetter) {
        this(connector, stmtSupplier, paramSetter, null);
    }

    @Override
    public void accept(T tuple) {
        executeStatement(tuple, null);
    }

    @Override
    public Iterable<R> apply(T tuple) {
        // lame impl for large result sets but will do for now.
        List<R> results = new ArrayList<>();
        executeStatement(tuple, results);
        return results;
    }
    
    private void executeStatement(T tuple, List<R> results) {
        nTuples++;
        try {
            logger.debug("executing statement nTuples={} nTuplesFailed={}", nTuples, nTuplesFailed);
            Connection cn = connector.getConnection(this);
            PreparedStatement stmt = getPreparedStatement(cn);
            paramSetter.setParameters(tuple, stmt);
            boolean hasResult = stmt.execute();
            if (resultsHandler != null) {
                if (!hasResult) {
                    resultsHandler.handleResults(tuple, null/*rs*/, null/*exc*/,
                            (result) -> results.add(result));
                }
                else {
                    do {
                        try (ResultSet rs = stmt.getResultSet()) {
                            resultsHandler.handleResults(tuple, rs, null/*exc*/,
                                    (result) -> results.add(result));
                        }
                    } while (stmt.getMoreResults());
                }
            }
        }
        catch (Exception e) {
            nTuplesFailed++;
            logger.trace("executing statement failed nTuples={} nTuplesFailed={}", nTuples, nTuplesFailed);
            if (resultsHandler != null) {
                try {
                    resultsHandler.handleResults(tuple, null/*rs*/, e, 
                            (result) -> results.add(result));
                }
                catch (Exception e2) {
                    logger.error("failure result handler failed", e2);
                }
            }
            closeStmt();
            connector.statementFailed(this, e);
        }
    }
    
    private PreparedStatement getPreparedStatement(Connection cn) throws SQLException {
        if (stmt == null) {
            stmt = stmtSupplier.get(cn);
        }
        return stmt;
    }

    @Override
    public void close() throws Exception {
        closeStmt();
        connector.unregister(this);
    }

}
