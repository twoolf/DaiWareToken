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

import com.google.common.collect.ImmutableSet;
import com.streamsets.pipeline.api.impl.Utils;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.channels.ReadableByteChannel;
import java.util.Set;

public class TestFileRef {
  private FileRef byteArrayFileRef;
  private byte[] data;

  private static class MockReadableByteChannel implements ReadableByteChannel {
    private InputStream is;
    private boolean isOpen;

    MockReadableByteChannel(InputStream is) {
      this.is = is;
      isOpen = true;
    }
    @Override
    public int read(ByteBuffer dst) throws IOException {
      if (isOpen) {
        Utils.checkArgument(dst.hasArray(), "Has to be a HeapByteBuffer");
        return is.read(dst.array());
      } else {
        throw new IOException("Channel not open for read.");
      }
    }

    @Override
    public boolean isOpen() {
      return false;
    }

    @Override
    public void close() throws IOException {
      is.close();
      isOpen = false;
    }
  }

  public static class ByteArrayRef extends FileRef {
    public final byte[] byteData;

    public ByteArrayRef(byte[] data) {
      super(data.length);
      byteData = data;
    }

    @Override
    @SuppressWarnings("unchecked")
    public <T extends AutoCloseable> Set<Class<T>> getSupportedStreamClasses() {
      return ImmutableSet.of((Class<T>) InputStream.class, (Class<T>) ReadableByteChannel.class);
    }

    @Override
    @SuppressWarnings("unchecked")
    public <T extends AutoCloseable> T createInputStream(Stage.Context context, Class<T> streamClassType) throws IOException {
      InputStream stream = new ByteArrayInputStream(byteData);
      if (streamClassType.equals(ReadableByteChannel.class)) {
        return (T)new MockReadableByteChannel(stream);
      }
      return (T) stream;
    }
  }

  @Before
  public void setup() {
    data = "This is streamsets file ref support".getBytes();
    byteArrayFileRef = new ByteArrayRef(data);
  }

  @Test
  public void checkFileRefMethods() throws Exception {
    Assert.assertTrue(byteArrayFileRef.getSupportedStreamClasses().containsAll(ImmutableSet.of(InputStream.class, ReadableByteChannel.class)));
    Assert.assertEquals(data.length, byteArrayFileRef.getBufferSize());
    try (InputStream is = byteArrayFileRef.createInputStream(null, InputStream.class)) {
      Assert.assertTrue(is instanceof ByteArrayInputStream);
    }
    try (ReadableByteChannel rc = byteArrayFileRef.createInputStream(null, ReadableByteChannel.class)) {
      Assert.assertTrue(rc instanceof MockReadableByteChannel);
    }
  }

  @Test
  public void checkInputStreamReadMethod() throws Exception {
    try (InputStream is = byteArrayFileRef.createInputStream(null, InputStream.class)) {
      byte[] b = new byte[data.length];
      int bytesRead = is.read(b);
      Assert.assertEquals(data.length, bytesRead);
      Assert.assertArrayEquals(data, b);
    }
  }

  @Test
  public void checkReadableByteChannelReadMethod() throws Exception {
    try (ReadableByteChannel rb = byteArrayFileRef.createInputStream(null, ReadableByteChannel.class)) {
      ByteBuffer buffer = ByteBuffer.wrap(new byte[data.length]);
      int bytesRead = rb.read(buffer);
      Assert.assertEquals(data.length, bytesRead);
      Assert.assertArrayEquals(data, buffer.array());
    }
  }
}
