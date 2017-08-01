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
package org.apache.edgent.test.connectors.common;

import java.io.File;
import java.nio.file.Path;
import java.nio.file.Paths;

/**
 * Utility for tests to get the path to something in the local git repository.
 */
public class TestRepoPath {

    /**
     * Get an absolute path to something in the local git repository.
     * <p>
     * Deals with implications of the different execution contexts:
     * eclipse/junit and ant/junit.
     * 
     * @param testProject the project (e.g., "connectors") that the
     *        test belongs to.
     * @param more more components
     * @return absolute path in the repository
     */
    public static String getPath(String testProject, String... more) {
        String pathStr = System.getProperty("user.dir");
        // Under eclipse/junit: path to project in repo: <repo>/<testProject>
        // Under ant/junit: <repo>/<testProject>/<project>/unittests/testrunxxxxxxx
        // Get the path to the <repo>
        Path path = new File(pathStr).toPath();
        do {
            if (path.endsWith(testProject)) {
                path = path.getParent();
                break;
            }
            path = path.getParent();
        } while (path != null);
        // add the components to the repo-path
        path = path.resolve(Paths.get(testProject, more));
        if (!path.toFile().exists())
            throw new IllegalArgumentException("File does not exist: "+path);
        return path.toString();
    }

}
