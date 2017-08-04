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

import com.streamsets.pipeline.api.impl.Utils;

/**
 * An <code>ELEval</code> instance evaluates Java EL expressions.
 * <p/>
 * In the context of a specific stage configuration, the <code>ELEval</code> uses the EL functions and EL constants
 * defined in the configuration.
 *
 * @see com.streamsets.pipeline.api.Stage.ELContext
 */
public abstract class ELEval {
  private static final ThreadLocal<ELVars> VARIABLES_IN_SCOPE_TL = new ThreadLocal<>();

  /**
   * Returns the stage configuration associated with the <code>ELEval</code> instance.
   *
   * @return the stage configuration associated with the <code>ELEval</code> instance.
   */
  public abstract String getConfigName();

  /**
   * Returns an empty <code>ELVars</code> instance.
   *
   * @return an empty <code>ELVars</code> instance.
   */
  public abstract ELVars createVariables();

  /**
   * Evaluates an EL.
   * <p/>
   * <b>IMPORTANT:</b> This is method is used by the implementation. It is not available for stages.
   *
   * @param vars the variables to be available for the evaluation.
   * @param el the EL string to evaluate.
   * @param returnType the class the EL evaluates to.
   * @return the evaluated EL as an instance of the specified return type.
   * @throws ELEvalException if the EL could not be evaluated.
   */
  protected abstract <T> T evaluate(ELVars vars, String el, Class<T> returnType) throws ELEvalException;

  /**
   * Evaluates an EL.
   *
   * @param vars the variables to be available for the evaluation.
   * @param el the EL string to evaluate.
   * @param returnType the class the EL evaluates to.
   * @return the evaluated EL as an instance of the specified return type.
   * @throws ELEvalException if the EL could not be evaluated.
   */
  public  <T> T eval(ELVars vars, String el, Class<T> returnType) throws ELEvalException {
    Utils.checkNotNull(vars, "vars");
    Utils.checkNotNull(el, "expression");
    Utils.checkNotNull(returnType, "returnType");
    VARIABLES_IN_SCOPE_TL.set(vars);
    try {
      return evaluate(vars, el, returnType);
    } finally {
      VARIABLES_IN_SCOPE_TL.set(null);
    }
  }

  /**
   * Returns the <code>ELVars</code> in scope while an EL is being evaluated.
   * <p/>
   * EL functions should use this method to get hold of the <code>ELVars</code> used to invoke the {@link #eval} method.
   * @return the <code>ELVars</code> in scope while an EL is being evaluated.
   */
  public static ELVars getVariablesInScope() {
    return VARIABLES_IN_SCOPE_TL.get();
  }
}