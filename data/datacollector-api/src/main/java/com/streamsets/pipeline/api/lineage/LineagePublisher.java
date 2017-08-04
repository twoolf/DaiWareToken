/**
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
package com.streamsets.pipeline.api.lineage;

import com.streamsets.pipeline.api.ErrorCode;

import java.util.List;

/**
 * Data collectors collects various lineage metadata for all pipelines while they are running. One can implement
 * subclass of LineagePublisher and configure Data Collector to use in order to propagate this data to a third
 * party system (such as Cloudera Navigator or Oracle Metadata Manager).
 */
public interface LineagePublisher {

  /**
   * Interface for configuration issues.
   *
   * @see Context#createConfigIssue(ErrorCode, Object...)
   */
  interface ConfigIssue {
  }

  /**
   * LineagePublisher Context that provides runtime information and services to the plugin.
   */
  public interface Context {

    /**
     * Returns the ID of the publisher, this ID is defined by configuration.
     *
     * @return the ID of the publisher.
     */
    String getId();

    /**
     * Creates a configuration issue for the publisher (at initialization time).
     *
     * @param errorCode the <code>ErrorCode</code> for the issue.
     * @param args the arguments for the <code>ErrorCode</code> message.
     * @return the configuration issue to report back.
     */
    ConfigIssue createConfigIssue(ErrorCode errorCode, Object... args);

    /**
     * Return value for given configuration option from data collector main configuration.
     *
     * Publisher have it's own namespace, so method won't be able to return generic SDC configuration.
     *
     * @param configName Configuration option name
     * @return String representation of the value or null if it's not defined.
     */
    String getConfig(String configName);

  }

  /**
   * Initializes the publisher.
   *
   * This method is called once during start up of data collector.
   *
   * If the publisher returns an empty list of {@link ConfigIssue}s then it is considered ready to process events.
   *
   * Else it is considered it is mis-configured or that there is a non-recoverable problem and the publisher is
   * not ready to process events, thus aborting the data collector initialization.
   *
   * @param context the publisher context.
   */
  List<ConfigIssue> init(Context context);

  /**
   * Process given list of events and push them to the lineage collector. Return true if and only if all events
   * were successfully written to the remote system.
   *
   * @param events List of lineage events that needs to be published
   */
  boolean publishEvents(List<LineageEvent> events);

  /**
   * Destroys the plugin. It should be used to release any resources held by the plugin after initialization or
   * processing.
   *
   * This method is called once when the data collector is being shutdown. After this method is called, the plugin
   * will not be called any more.
   *
   * This method is also called after a failed initialization to allow releasing resources created before the
   * initialization failed.
   */
  void destroy();
}
