/*
 * Copyright 2017 StreamSets Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.streamsets.pipeline.api.credential;

import com.streamsets.pipeline.api.ErrorCode;
import com.streamsets.pipeline.api.StageException;

import java.util.List;

/**
 * A credential store gives programmatic access, via a reference name, to sensitive values such as system passwords.
 */
public interface CredentialStore {

  /**
   * Marker interface for configuration issues.
   */
  interface ConfigIssue {
  }

  /**
   * Context provided to credential store implementations at initialization time.
   */
  interface Context {

    /**
     * Returns the ID of the store, this ID is defined by configuration.
     *
     * @return the ID of the store.
     */
    String getId();

    /**
     * Creates a configuration issue (at initialization time).
     *
     * @param errorCode error code of the issue.
     * @param args arguments for the message template of the given error code.
     * @return The configuration issue to report back.
     */
    ConfigIssue createConfigIssue(ErrorCode errorCode, Object... args);

    /**
     * Return value for given configuration option from data collector main configuration.
     * <p/>
     * The credential store have it's own configuration namespace, this method won't be able to access other SDC
     * configuration values.
     *
     * @param configName Configuration option name
     * @return String representation of the value or null if it's not defined.
     */
    String getConfig(String configName);
  }

  /**
   * Initializes the credential store.
   * <p/>
   * This method is called once during start up of data collector.
   * <p/>
   * If the publisher returns an empty list of {@link ConfigIssue}s then it is considered successful.
   * <p/>
   * Else it is considered it is mis-configured or that there is a non-recoverable problem and the credential store
   * is not ready for use, thus aborting the data collector initialization.
   *
   * @param context the publisher context.
   */
  List<ConfigIssue> init(Context context);

  /**
   * Returns a credential value associated to the given name.
   * @param group group the user must belong to retrieve the credential.
   * @param name reference name for the credential.
   * @param credentialStoreOptions options specific to the credential store implementation. Implementations must work
   * with NULL and empty options.
   * @return the credential value, or NULL if not found.
   * @throws StageException thrown if the credential could not be retrieved because of permissions or other reason.
   */
  CredentialValue get(String group, String name, String credentialStoreOptions) throws StageException;

  /**
   * Destroys the credential store.
   * <p/>
   * This method is called once when the data collector is being shutdown. After this method is called, the credential
   * store will not be called any more.
   * <p/>
   * This method is also called after a failed initialization to allow releasing resources created before the
   * initialization failed.
   */
  void destroy();

}
