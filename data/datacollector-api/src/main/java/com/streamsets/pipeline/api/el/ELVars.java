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
package com.streamsets.pipeline.api.el;

/**
 * The <code>ELVars</code> hold all constants, variables and context variables available during an EL evaluation.
 * <p/>
 *
 * @see ELEval
 * @see ELEval#getVariablesInScope()
 */
public interface ELVars {

  /**
   * Returns the specified EL constant.
   * <p/>
   * The EL constant has been defined in one of the classes indicated in the
   * {@link com.streamsets.pipeline.api.ConfigDef#elDefs()} attribute, or in a class annotated with
   * {@link com.streamsets.pipeline.api.ElDef} and available to the stage library.
   *
   * @param name the EL constant name.
   * @return the specified EL constant, or <code>NULL</code> if not defined.
   */
  public Object getConstant(String name);

  /**
   * Adds a variable to be available in the EL being evaluated.
   *
   * @param name variable name.
   * @param value variable value.
   */
  public void addVariable(String name, Object value);

  /**
   * Adds a context variable to be available within the evaluation of an EL, but not in the EL itself.
   * <p/>
   * Method associated with an EL function have access to the context variables via the
   * {@link ELEval#getVariablesInScope()} method.
   *
   * @param name context variable name.
   * @param value context variable value.
   */
  public void addContextVariable(String name, Object value);

  /**
   * Indicates if a variable is defined or not.
   * @param name variable name.
   * @return if the variable is defined or not.
   */
  public boolean hasVariable(String name);

  /**
   * Returns the variable value.
   * @param name variable name.
   * @return the variable value.
   */
  public Object getVariable(String name);

  /**
   * Indicates if a context variable is defined or not.
   * @param name variable name.
   * @return if the  context variable is defined or not.
   */
  public boolean hasContextVariable(String name);

  /**
   * Returns the context variable value.
   * @param name context variable name.
   * @return the context variable value.
   */
  public Object getContextVariable(String name);
}
