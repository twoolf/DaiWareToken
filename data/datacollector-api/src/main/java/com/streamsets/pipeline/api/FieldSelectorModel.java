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
 * Annotation for configuration variables that need to be populated with record field names. It supports single
 * and multi value selections.
 * <p/>
 * The configuration definition must be of type {@link com.streamsets.pipeline.api.ConfigDef.Type#MODEL}.
 * <p/>
 * If defined to support multi value selections, the variable must be of type <code>java.util.List&lt;?></code>. If
 * defined for single value the variable should be of type {@link String}.
 */
@Retention(RetentionPolicy.RUNTIME)
@java.lang.annotation.Target(ElementType.FIELD)
public @interface FieldSelectorModel {

  /**
   * Indicates if the selection should be single value (default) or multi value.
   */
  boolean singleValued() default false;
}
