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

import com.streamsets.pipeline.api.FileRef;
import com.streamsets.pipeline.api.TestFileRef;
import com.streamsets.pipeline.api.base.Errors;
import org.junit.Assert;
import org.junit.Test;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;

public class TestFileRefTypeSupport {

  @Test
  public void testCreate() {
    FileRefTypeSupport frTs = new FileRefTypeSupport();
    byte[] data = "This is streamsets file ref support".getBytes();
    TestFileRef.ByteArrayRef byteArrayRef = new TestFileRef.ByteArrayRef(data);
    TestFileRef.ByteArrayRef createdByteArrayRef = (TestFileRef.ByteArrayRef) frTs.create(byteArrayRef);
    Assert.assertEquals(byteArrayRef, createdByteArrayRef);
    Assert.assertEquals(byteArrayRef.byteData, createdByteArrayRef.byteData);
  }

  @Test
  public void testGet() throws Exception {
    FileRefTypeSupport frTs = new FileRefTypeSupport();
    byte[] data = "This is streamsets file ref support".getBytes();
    TestFileRef.ByteArrayRef o = new TestFileRef.ByteArrayRef(data);
    Assert.assertSame(o, frTs.get(o));
    Assert.assertSame(o.byteData, ((TestFileRef.ByteArrayRef)frTs.get(o)).byteData);
  }

  @Test
  public void testClone() {
    FileRefTypeSupport frTs = new FileRefTypeSupport();
    byte[] data = "This is streamsets file ref support".getBytes();
    TestFileRef.ByteArrayRef byteArrayRef = new TestFileRef.ByteArrayRef(data);
    TestFileRef.ByteArrayRef clonedByteArrayRef = (TestFileRef.ByteArrayRef) frTs.clone(byteArrayRef);
    Assert.assertSame(byteArrayRef, clonedByteArrayRef);
    Assert.assertSame(byteArrayRef.byteData, clonedByteArrayRef.byteData);
  }

  @Test
  public void testConvertValid() {
    FileRefTypeSupport frTs = new FileRefTypeSupport();
    byte[] data = "This is streamsets file ref support".getBytes();
    TestFileRef.ByteArrayRef byteArrayRef = new TestFileRef.ByteArrayRef(data);
    Assert.assertSame(byteArrayRef, frTs.convert(byteArrayRef));
  }

  private void testConvertToFileRefInValid(Object object) {
    try {
      new FileRefTypeSupport().convert(object);
      Assert.fail("Convert to File Ref Type from " + object.getClass().getName() + " should fail. ");
    } catch (IllegalArgumentException e) {
      Assert.assertEquals(Utils.format(Errors.API_23.getMessage(), object.getClass().getName()), e.getMessage());
    }
  }

  @Test
  public void testConvertToFileRefInvalid() {
    testConvertToFileRefInValid(new Exception());
    testConvertToFileRefInValid(Short.valueOf("1"));
    testConvertToFileRefInValid(Integer.valueOf(1));
    testConvertToFileRefInValid(Long.valueOf(1));
    testConvertToFileRefInValid(Float.valueOf(1.5F));
    testConvertToFileRefInValid(Double.valueOf(1.34));
    testConvertToFileRefInValid(new BigDecimal(45.3232));
    testConvertToFileRefInValid(Byte.valueOf("1"));
    testConvertToFileRefInValid(Character.valueOf('a'));
    testConvertToFileRefInValid(true);
    testConvertToFileRefInValid("This is a byte array".getBytes());
    testConvertToFileRefInValid("This is a string");
    testConvertToFileRefInValid(new LinkedHashMap<>());
    testConvertToFileRefInValid(new ArrayList<>());
    testConvertToFileRefInValid(new HashMap<>());
  }

  private void testConvertFromFilRefInvalid(TypeSupport targetTypeSupport, FileRef fileRef) {
    try {
      new FileRefTypeSupport().convert(fileRef, targetTypeSupport);
      Assert.fail("Convert File Ref Object to " + targetTypeSupport + " should fail.");
    } catch (IllegalArgumentException e) {
      Assert.assertEquals(Utils.format(Errors.API_24.getMessage(), targetTypeSupport), e.getMessage());
    }
  }

  @Test
  public void testConvertFromFilRefInvalid() {
    FileRef fileRef = new TestFileRef.ByteArrayRef("This is file ref Field".getBytes());
    testConvertFromFilRefInvalid(new IntegerTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new LongTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new FloatTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new DoubleTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new DecimalTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new ByteTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new CharTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new BooleanTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new ByteArrayTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new StringTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new ListMapTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new ListTypeSupport(), fileRef);
    testConvertFromFilRefInvalid(new MapTypeSupport(), fileRef);
  }

}
