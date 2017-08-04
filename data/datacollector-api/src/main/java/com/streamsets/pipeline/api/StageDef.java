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


import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Annotation to declare Data Collector stages classes. Classes must implement {@link Source}, {@link Processor} or
 * {@link Target}.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface StageDef {

  //enum for processors using LanePredicateMapping configurations

  // we are using the annotation for reference purposes only.
  // the annotation processor does not work on this maven project
  // we have a hardcoded 'datacollector-resource-bundles.json' file in resources

  /**
   * Enumeration to use in {@link #outputStreams()} to indicate that the output streams are variable, driven by the
   * stage configuration specified in {@link #outputStreamsDrivenByConfig()}.
   */
  @GenerateResourceBundle
  enum VariableOutputStreams implements Label {
    ;

    @Override
    public String getLabel() {
      return null;
    }
  }

  /**
   * Default enumeration used by stages having a single output stream.
   */
  @GenerateResourceBundle
  public enum DefaultOutputStreams implements Label {
    OUTPUT("Output");

    private final String label;

    DefaultOutputStreams(String label) {
      this.label = label;
    }

    @Override
    public String getLabel() {
      return label;
    }
  }

  /**
   * Indicates the version of the stage.
   * <p/>
   * The version is used to track the configuration of a stage definition and any necessary upgrade via a
   * {@link StageUpgrader}
   */
  int version();

  /**
   * Indicates the UI default label for the stage.
   */
  String label();

  /**
   * Indicates the UI default description for the stage.
   */
  String description() default "";

  /**
   * Indicates the UI icon for the stage.
   */
  String icon() default "";

  /**
   * If the number of output streams is driven a stage configuration (it must be a {@link java.util.List}), the name
   * of the configuration must be indicated here.
   */
  String outputStreamsDrivenByConfig() default ""; //selector  case

  /**
   * Indicates an enum (implementing {@link Label}) that defines the output streams names and UIL labels for the stage.
   * If not set the stage has one output with name <i>output</i> and label <i>Output</i>.
   * <p/>
   * If setting {@link #outputStreamsDrivenByConfig()} the {@link VariableOutputStreams} enum must be used.
   */
  Class<? extends Label> outputStreams() default DefaultOutputStreams.class;

  /**
   * Indicates the stage supported execution modes.
   */
  ExecutionMode[] execution() default { ExecutionMode.STANDALONE, ExecutionMode.CLUSTER_BATCH, ExecutionMode.CLUSTER_YARN_STREAMING, ExecutionMode.CLUSTER_MESOS_STREAMING };

  /**
   * Indicates wheather stage (Origin) supports resetting offset
   */
  boolean resetOffset() default false;

  boolean recordsByRef() default false;

  /**
   * Indicates each stage instance should use a private classloader.
   */
  boolean privateClassLoader() default false;

  /**
   * Indicates the upgrader implementation class to use to upgrade stage configurations for older stage versions.
   */
  Class<? extends StageUpgrader> upgrader() default StageUpgrader.Default.class;

  /**
   * Indicates an array of regex patterns of the stage library JARs that must be used for the slave Data Collectors
   * at bootstrap time.
   * <p/>
   * <b>IMPORTANT:</b> applicable to cluster mode executions only.
   */
  String[] libJarsRegex() default {};

  /**
   * Relative path to online help for this stage.
   */
  String onlineHelpRefUrl();

  /**
   * Indicates if this stage can produce events.
   */
  boolean producesEvents() default false;
}
