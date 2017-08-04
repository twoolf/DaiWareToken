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

/**
 * Annotation to hide stage configurations in the UI.
 * <p/>
 * Useful to hide/disable built in configurations fields, preconditions and on error record; as well as user defined
 * configurations when sub-classing stages to perform specialized tasks.
 */
@Retention(RetentionPolicy.RUNTIME)
@java.lang.annotation.Target(ElementType.TYPE)
public @interface HideConfigs {
  /**
   * Hides system defined 'required fields' and preconditions configurations from the UI.
   */
  boolean preconditions() default false;

  /**
   * Hides system defined 'on error record' configuration from the UI.
   */
  boolean onErrorRecord() default false;

  /**
   * Hides the specifiedd user defined configurations from the UI.
   */
  String[] value() default {};
}
