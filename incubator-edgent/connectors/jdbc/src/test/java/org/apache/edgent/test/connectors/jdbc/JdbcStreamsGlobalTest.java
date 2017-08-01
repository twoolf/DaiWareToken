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
package org.apache.edgent.test.connectors.jdbc;

import java.util.ArrayList;
import java.util.List;

/**
 * JdbcStreams connector globalization tests.
 * <p>
 * The tests use Apache Embedded Derby as the backing dbms.
 * The Oracle JDK includes Derby in $JAVA_HOME/db.
 * Manually install Derby for other JDKs if required.
 * Arrange for the classpath to be configured by one of:
 * <ul>
 * <li>set the DERBY_HOME environment variable.  build.xml adds
 *     $DERBY_HOME/lib/derby.jar to the classpath when running the tests.</li>
 * <li>manually add derby.jar to the classpath</li>
 * </ul>
 * The tests are "skipped" if the dbms's jdbc driver can't be found.
 */
public class JdbcStreamsGlobalTest extends JdbcStreamsTest {

    private static final List<Person> globalPersonList = new ArrayList<>();
    static {
        globalPersonList.add(new Person(1, "约翰", "李", "male", 35));
        globalPersonList.add(new Person(2, "简", "李", "female", 29));
        globalPersonList.add(new Person(3, "比利", "周", "male", 3));
    }
    private static final List<PersonId> globalPersonIdList = new ArrayList<>();
    static {
        for(Person p : globalPersonList) {
            globalPersonIdList.add(new PersonId(p.id));
        }
    }

    public List<Person> getPersonList() {
        return globalPersonList;
    }

    public List<PersonId> getPersonIdList() {
        return globalPersonIdList;
    }

}
