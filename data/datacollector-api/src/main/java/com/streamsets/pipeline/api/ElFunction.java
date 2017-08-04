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
import java.lang.annotation.Target;
import java.lang.annotation.RetentionPolicy;

/**
 * Defines an EL function.
 * <p/>
 * The method must be <code>public</code>, <code>static</code>. Parameters of the function should use the
 * {@link ElParam} annotation.
 */
@Retention(RetentionPolicy.RUNTIME)
@Target({ElementType.METHOD})
public @interface ElFunction {

  /**
   * Function prefix to be used within ELs.
   */
  String prefix() default "";

  /**
   * Function name to be used within ELs.
   */
  String name();

  /**
   * Description of the constant, for the UI help.
   */
  String description() default "";

  /**
   * Defines the EL function can only be used in implicit configurations.
   */
  boolean implicitOnly() default false;

}
