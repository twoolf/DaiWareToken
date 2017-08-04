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
package com.streamsets.pipeline.api.impl;

import org.junit.Assert;
import org.junit.Test;

import java.math.BigDecimal;
import java.math.BigInteger;

public class TestDecimalTypeSupport {

  @Test
  public void testCreate() {
    DecimalTypeSupport ts = new DecimalTypeSupport();
    BigDecimal o = BigDecimal.ONE;
    Assert.assertSame(o, ts.create(o));
  }

  @Test
  public void testGet() {
    DecimalTypeSupport ts = new DecimalTypeSupport();
    BigDecimal o = BigDecimal.ONE;
    Assert.assertSame(o, ts.get(o));
  }

  @Test
  public void testClone() {
    DecimalTypeSupport ts = new DecimalTypeSupport();
    BigDecimal o = BigDecimal.ONE;
    Assert.assertSame(o, ts.clone(o));
  }

  @Test
  public void testConvertValid() {
    DecimalTypeSupport support = new DecimalTypeSupport();
    Assert.assertEquals(BigDecimal.ONE, support.convert("1"));
    Assert.assertEquals(BigDecimal.ONE, support.convert((byte)1));
    Assert.assertEquals(BigDecimal.ONE, support.convert((short)1));
    Assert.assertEquals(BigDecimal.ONE, support.convert(1));
    Assert.assertEquals(BigDecimal.ONE, support.convert((long)1));
    // Should not convert to 1 since 1 is less precise than 1.0
    Assert.assertEquals(new BigDecimal("1.0"), support.convert(1.0f));
    Assert.assertEquals(new BigDecimal("1.0"), support.convert(1.0d));
    Assert.assertEquals(BigDecimal.ONE, support.convert(BigInteger.ONE));
    Assert.assertEquals(BigDecimal.ONE, support.convert(BigDecimal.ONE));
  }

  @Test(expected = IllegalArgumentException.class)
  public void testConvertInValid() {
    new DecimalTypeSupport().convert(new Exception());
  }

}
