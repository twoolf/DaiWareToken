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
package com.streamsets.pipeline.api;

/**
 * Defines the execution modes of a pipeline.
 * <p/>
 * It is used in stage definitions to restrict the execution modes the stage supports.
 * <p/>
 * Accessible via the stage context to know the current execution mode of the pipeline.
 *
 * @see StageDef#execution()
 * @see Stage.Context#getExecutionMode()
 */
public enum ExecutionMode implements Label {
  STANDALONE("Standalone"),
  @Deprecated
  CLUSTER("Cluster"), //Kept for backward compatibility - replaced by CLUSTER_BATCH and CLUSTER_STREAMING
  CLUSTER_BATCH("Cluster Batch"),
  CLUSTER_YARN_STREAMING("Cluster Yarn Streaming"),
  CLUSTER_MESOS_STREAMING("Cluster Mesos Streaming"),
  SLAVE("Slave")
  ;
  private final String label;

  ExecutionMode(String label) {
    this.label = label;
  }

  /**
   * Returns the default value of localizable label, for the UI, of the execution mode enum.
   *
   * @return the default value of localizable label.
   */
  @Override
  public String getLabel() {
    return label;
  }
}
