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

import com.streamsets.pipeline.api.impl.BooleanTypeSupport;
import com.streamsets.pipeline.api.impl.ByteArrayTypeSupport;
import com.streamsets.pipeline.api.impl.ByteTypeSupport;
import com.streamsets.pipeline.api.impl.CharTypeSupport;
import com.streamsets.pipeline.api.impl.CreateByRef;
import com.streamsets.pipeline.api.impl.DateTypeSupport;
import com.streamsets.pipeline.api.impl.DecimalTypeSupport;
import com.streamsets.pipeline.api.impl.DoubleTypeSupport;
import com.streamsets.pipeline.api.impl.FloatTypeSupport;
import com.streamsets.pipeline.api.impl.IntegerTypeSupport;
import com.streamsets.pipeline.api.impl.ListMapTypeSupport;
import com.streamsets.pipeline.api.impl.ListTypeSupport;
import com.streamsets.pipeline.api.impl.LongTypeSupport;
import com.streamsets.pipeline.api.impl.MapTypeSupport;
import com.streamsets.pipeline.api.impl.FileRefTypeSupport;
import com.streamsets.pipeline.api.impl.ShortTypeSupport;
import com.streamsets.pipeline.api.impl.StringTypeSupport;
import com.streamsets.pipeline.api.impl.TypeSupport;
import com.streamsets.pipeline.api.impl.Utils;

import java.math.BigDecimal;
import java.util.Collections;
import java.util.Date;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * A <code>Field</code> is the basic building block to represent data within Data Collector stages. Stages receive and
 * produce batches of {@link Record}s, a <code>Record</code> as a <code>Field</code> to hold its data.
 * <p/>
 * A <code>Field</code> is a data structure construct consisting of a type/value pair. It supports basic types
 * (i.e. numbers, strings, dates) as well as structure oriented types (i.e. Maps and Lists).
 * <p/>
 * <code>Field</code> values can be automatically converted to compatible Java primitive types and classes.
 * <code>Field</code> values can be <code>NULL</code> (while preserving the type).
 * <p/>
 * The {@link Type} enumeration defines all the supported types.
 * <p/>
 * Except for the Collection based types, <code>Field</code> values are immutable. Collection <code>Field</code> values
 * can be modified using the corresponding Collection manipulation API.
 * <p/>
 * <b>NOTE:</b> Java <code>Date</code> and <code>byte[]</code> are not immutable. <code>Field</code> makes immutable
 * by performing a copy on <code>create()</code> and on <code>get()</code>. This means that if a <code>Date</code> or
 * <code>byte[]</code> instance obtained from a <code>Field</code> is modified, the actual value stored in the
 * <code>Field</code> is not modified.
 * <p/>
 * The {@link #hashCode}, {@link #equals} and {@link #clone} methods work in deep operation mode on the
 * <code>Field</code>.
 *
 * @see Record
 */
public class Field implements Cloneable {

  private Type type;
  private Object value;
  private Map<String, String> attributes;

  /**
   * Enum defining all <code>Field</code> types.
   */
  public enum Type {
    BOOLEAN(new BooleanTypeSupport()),
    CHAR(new CharTypeSupport()),
    BYTE(new ByteTypeSupport()),
    SHORT(new ShortTypeSupport()),
    INTEGER(new IntegerTypeSupport()),
    LONG(new LongTypeSupport()),
    FLOAT(new FloatTypeSupport()),
    DOUBLE(new DoubleTypeSupport()),
    DATE(new DateTypeSupport()),
    DATETIME(new DateTypeSupport()),
    TIME(new DateTypeSupport()),
    DECIMAL(new DecimalTypeSupport()),
    STRING(new StringTypeSupport()),
    FILE_REF(new FileRefTypeSupport()),
    BYTE_ARRAY(new ByteArrayTypeSupport()),
    MAP(new MapTypeSupport()),
    LIST(new ListTypeSupport()),
    LIST_MAP(new ListMapTypeSupport());

    final transient TypeSupport<?> supporter;

    Type(TypeSupport<?> supporter) {
      this.supporter = supporter;
    }

    private Object convert(Object value) {
      return (value != null) ? supporter.convert(value) : null;
    }

    private Object convert(Object value, Type targetType) {
      return (value != null) ? supporter.convert(value, targetType.supporter) : null;
    }

    private boolean equals(Object value1, Object value2) {
      return supporter.equals(value1, value2);
    }

    @SuppressWarnings("unchecked")
    private <T> T constructorCopy(T value) {
      return (value != null) ? (T) supporter.create(value) : null;
    }

    @SuppressWarnings("unchecked")
    private <T> T getReference(T value) {
      return (value != null) ? (T) supporter.get(value) : null;
    }

    private String toString(Object value) {
      return Utils.format("Field[{}:{}]", this, value);
    }

    public boolean isOneOf(Type ...types) {
      if(types == null) {
        return false;
      }

      for(Type t : types) {
        if(this == t) return true;
      }
      return false;
    }
  }

  // need default constructor for deserialization purposes (Kryo)
  private Field() {
  }

  private Field(Type type, Object value) {
    this(type, value, null);
  }

  private Field(Type type, Object value, Map<String, String> attributes) {
    this.type = type;
    this.value = CreateByRef.isByRef() ? value : type.constructorCopy(value);
    if (attributes != null) {
      this.attributes = new LinkedHashMap<>(attributes);
    }
  }

  /**
   * @deprecated DO NOT USE, TO BE REMOVED
   */
  @Deprecated
  public static <T> Field create(Field field, T value) {
    return create(Utils.checkNotNull(field, "field").getType(), value);
  }

  /**
   * Creates a <code>BOOLEAN</code> field.
   *
   * @param v value.
   *
   * @return a <code>boolean Field</code> with the given value.
   */
  public static Field create(boolean v) {
    return new Field(Type.BOOLEAN, v);
  }

  /**
   * Creates a <code>CHAR</code> field.
   *
   * @param v value.
   *
   * @return a <code>char Field</code> with the given value.
   */
  public static Field create(char v) {
    return new Field(Type.CHAR, v);
  }

  /**
   * Creates a <code>BYTE</code> field.
   *
   * @param v value.
   *
   * @return a <code>byte Field</code> with the given value.
   */
  public static Field create(byte v) {
    return new Field(Type.BYTE, v);
  }

  /**
   * Creates a <code>SHORT</code> field.
   *
   * @param v value.
   *
   * @return a <code>short Field</code> with the given value.
   */
  public static Field create(short v) {
    return new Field(Type.SHORT, v);
  }

  /**
   * Creates a <code>INTEGER</code> field.
   *
   * @param v value.
   *
   * @return a <code>int Field</code> with the given value.
   */
  public static Field create(int v) {
    return new Field(Type.INTEGER, v);
  }

  /**
   * Creates a <code>LONG</code> field.
   *
   * @param v value.
   *
   * @return a <code>long Field</code> with the given value.
   */
  public static Field create(long v) {
    return new Field(Type.LONG, v);
  }

  /**
   * Creates a <code>FLOAT</code> field.
   *
   * @param v value.
   *
   * @return a <code>float Field</code> with the given value.
   */
  public static Field create(float v) {
    return new Field(Type.FLOAT, v);
  }

  /**
   * Creates a <code>DOUBLE</code> field.
   *
   * @param v value.
   *
   * @return a <code>double Field</code> with the given value.
   */
  public static Field create(double v) {
    return new Field(Type.DOUBLE, v);
  }

  /**
   * Creates a <code>DECIMAL</code> Field.
   *
   * @param v value.
   *
   * @return a <code>BigDecimal Field</code> with the given value.
   */
  public static Field create(BigDecimal v) {
    return new Field(Type.DECIMAL, v);
  }

  /**
   * Creates a <code>STRING</code> field.
   *
   * @param v value.
   *
   * @return a <code>String Field</code> with the given value.
   */
  public static Field create(String v) {
    return new Field(Type.STRING, v);
  }

  /**
   * Creates a <code>BYTE_ARRAY</code> field.
   *
   * @param v value.
   *
   * @return a <code>byte array Field</code> with the given value.
   */
  public static Field create(byte[] v) {
    return new Field(Type.BYTE_ARRAY, v);
  }

  /**
   * Creates a <code>DATE</code> field.
   *
   * @param v value.
   *
   * @return a <code>Date Field</code> with the given value.
   */
  public static Field createDate(Date v) {
    return new Field(Type.DATE, v);
  }

  /**
   * Creates a <code>DATETIME</code> field.
   * <p/>
   * Java does not have Datetime field, the intent of the {@link Type#DATETIME} type is to use only the year, month
   * and day information of the Date.
   *
   * @param v value.
   *
   * @return a <code>Datetime Field</code> with the given value.
   */
  public static Field createDatetime(Date v) {
    return new Field(Type.DATETIME, v);
  }

  /**
   * Creates a <code>TIME</code> field.
   * <p/>
   * Java does not have Time field, the intent of the {@link Type#DATETIME} type is to use only the hour, minute,
   * and second information of the Date.
   *
   * @param v value.
   *
   * @return a <code>Time Field</code> with the given value.
   */
  public static Field createTime(Date v) {
    return new Field(Type.TIME, v);
  }

  /**
   * Creates a <code>LIST</code> field. The keys of the Map must be of <code>String</code> type and the values must be
   * of <code>Field</code> type, <code>NULL</code> values are not allowed as key or values.
   * <p/>
   * This method performs a deep copy of the given Map.
   *
   * @param v value.
   *
   * @return a <code>Map Field</code> with the given value.
   */
  public static Field create(Map<String, Field> v) {
    return new Field(Type.MAP, v);
  }

  /**
   * Creates a <code>LIST</code> field. The values of the List must be of <code>Field</code> type, <code>NULL</code>
   * values are not allowed.
   * <p/>
   * This method performs a deep copy of the given List.
   * <p/>
   *
   * @param v value.
   *
   * @return a <code>List Field</code> with the given value.
   */
  public static Field create(List<Field> v) {
    return new Field(Type.LIST, v);
  }

  /**
   * Creates a <code>LIST_MAP</code> field. The keys of the ordered Map must be of <code>String</code> type and the
   * values must be of <code>Field</code> type, <code>NULL</code> values are not allowed as key or values.
   * <p/>
   * This method performs a deep copy of the ordered Map.
   *
   * @param v value.
   *
   * @return a <code>List-Map Field</code> with the given value.
   */
  public static Field createListMap(LinkedHashMap<String, Field> v) {
    return new Field(Type.LIST_MAP, v);
  }

  /**
   * Creates a <code>FILE_REF</code> field.
   *
   * @param v value.
   *
   * @return a <code>FILE_REF Field</code> with the given value.
   */
  public static Field create(FileRef v) {
    return new Field(Type.FILE_REF, v);
  }

  /**
   * Creates a field of a given type, the value is converted to the specified type.  The attributes are set to null.
   * <p/>
   * If the type is <code>MAP</code>, <code>LIST</code> or <code>LIST_MAP</code> this method performs a deep copy of
   * the value.
   *
   * @param type the type of the field to create.
   * @param value the value to set in the field.
   * @return the created Field.
   * @throws IllegalArgumentException if the value cannot be converted to the specified type.
   */
  public static <T> Field create(Type type, T value) {
    return create(type, value, null);
  }

  /**
   * Creates a field of a given type, the value is converted to the specified type.
   * <p/>
   * If the type is <code>MAP</code>, <code>LIST</code> or <code>LIST_MAP</code> this method performs a deep copy of
   * the value.
   *
   * @param type the type of the field to create.
   * @param value the value to set in the field.
   * @param attributes the field-level attributes (can be null)
   * @return the created Field.
   * @throws IllegalArgumentException if the value cannot be converted to the specified type.
   */
  public static <T> Field create(Type type, T value, Map<String, String> attributes) {
    return new Field(Utils.checkNotNull(type, "type"), type.convert(value), attributes);
  }

  /**
   * Returns the type of the field.
   *
   * @return the type of the field.
   */
  public Type getType() {
    return type;
  }

  /**
   * Returns the value of the field.
   * <p/>
   * For fields with <code>DATE</code>, <code>DATETIME</code> or <code>BYTE_ARRAY</code> type, a copy of the value is
   * returned. If the returned instance is modified, the changes are not reflected in the field value itself.
   * <p/>
   * For fields with a collection type (<code>MAP</code>, <code>LIST</code> and <code>LIST_MAP</code>), a reference to
   * the value is returned, any changes to the returned collection object will be reflected in the field value itself.
   * <p/>
   * For all other types, the values are immutable.
   * <p/>
   * @return the value of the field.
   */
  public Object getValue() {
    return type.getReference(value);
  }

  /**
   * Returns the boolean value of the field.
   *
   * @return the boolean value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to boolean.
   */
  public boolean getValueAsBoolean() {
    return (boolean) type.convert(getValue(), Type.BOOLEAN);
  }

  /**
   * Returns the char value of the field.
   *
   * @return the char value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to char.
   */
  public char getValueAsChar() {
    return (char) type.convert(getValue(), Type.CHAR);
  }

  /**
   * Returns the byte value of the field.
   *
   * @return the byte value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to byte.
   */
  public byte getValueAsByte() {
    return (byte) type.convert(getValue(), Type.BYTE);
  }

  /**
   * Returns the short value of the field.
   *
   * @return the short value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to short.
   */
  public short getValueAsShort() {
    return (short) type.convert(getValue(), Type.SHORT);
  }

  /**
   * Returns the int value of the field.
   *
   * @return the int value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to int.
   */
  public int getValueAsInteger() {
    return (int) type.convert(getValue(), Type.INTEGER);
  }

  /**
   * Returns the long value of the field.
   *
   * @return the long value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to long.
   */
  public long getValueAsLong() {
    return (long) type.convert(getValue(), Type.LONG);
  }

  /**
   * Returns the float value of the field.
   *
   * @return the float value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to float.
   */
  public float getValueAsFloat() {
    return (float) type.convert(getValue(), Type.FLOAT);
  }

  /**
   * Returns the double value of the field.
   *
   * @return the double value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to double.
   */
  public double getValueAsDouble() {
    return (double) type.convert(getValue(), Type.DOUBLE);
  }

  /**
   * Returns the Date value of the field.
   *
   * @return the Date value of the field. It returns a copy of the value.
   * @throws IllegalArgumentException if the value cannot be converted to Date.
   */
  public Date getValueAsDate() {
    return (Date) type.convert(getValue(), Type.DATE);
  }

  /**
   * Returns the Date value of the field.
   *
   * @return the Date value of the field. it returns a copy of the value.
   * @throws IllegalArgumentException if the value cannot be converted to Date.
   */
  public Date getValueAsDatetime() {
    return (Date) type.convert(getValue(), Type.DATE);
  }

  /**
   * Returns the Date value of the field.
   *
   * @return the Date value of the field. it returns a copy of the value.
   * @throws IllegalArgumentException if the value cannot be converted to Date.
   */
  public Date getValueAsTime() {
    return (Date) type.convert(getValue(), Type.TIME);
  }

  /**
   * Returns the BigDecimal value of the field.
   *
   * @return the BigDecimal value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to BigDecimal.
   */
  public BigDecimal getValueAsDecimal() {
    return (BigDecimal) type.convert(getValue(), Type.DECIMAL);
  }

  /**
   * Returns the String value of the field.
   *
   * @return the String value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to String.
   */
  public String getValueAsString() {
    return (String) type.convert(getValue(), Type.STRING);
  }

  /**
   * Returns the byte array value of the field.
   *
   * @return the byte array value of the field. It returns a copy of the value.
   * @throws IllegalArgumentException if the value cannot be converted to byte array.
   */
  public byte[] getValueAsByteArray() {
    return (byte[]) type.convert(getValue(), Type.BYTE_ARRAY);
  }

  /**
   * Returns the Map value of the field.
   *
   * @return the Map value of the field. It returns a reference of the value both for <code>MAP</code> and
   * <code>LIST_MAP</code>.
   * @throws IllegalArgumentException if the value cannot be converted to Map.
   */
  @SuppressWarnings("unchecked")
  public Map<String, Field> getValueAsMap() {
    return (Map<String, Field>) type.convert(getValue(), Type.MAP);
  }

  /**
   * Returns the List value of the field.
   *
   * @return the List value of the field. It returns a reference of the value if the type is <code>LIST</code>, if
   * the type is <code>LIST_MAP</code> it returns a copy of the value.
   * @throws IllegalArgumentException if the value cannot be converted to List.
   */
  @SuppressWarnings("unchecked")
  public List<Field> getValueAsList() {
    return (List<Field>) type.convert(getValue(), Type.LIST);
  }

  /**
   * Returns the ordered Map value of the field.
   *
   * @return the ordered Map value of the field. It returns a reference of the value.
   * @throws IllegalArgumentException if the value cannot be converted to ordered Map.
   */
  @SuppressWarnings("unchecked")
  public LinkedHashMap<String, Field> getValueAsListMap() {
    return (LinkedHashMap<String, Field>) type.convert(getValue(), Type.LIST_MAP);
  }

  /**
   * Returns the {@link FileRef} value of the field.
   *
   * @return the {@link FileRef} value of the field.
   * @throws IllegalArgumentException if the value cannot be converted to {@link FileRef}.
   */
  @SuppressWarnings("unchecked")
  public FileRef getValueAsFileRef() {
    return (FileRef) type.convert(getValue(), Type.FILE_REF);
  }


  /**
   * Returns the list of user defined attribute names.
   *
   * @return the list of user defined attribute names, if there are none it returns an empty set.
   */
  public Set<String> getAttributeNames() {
    if (attributes == null) {
      return Collections.emptySet();
    } else {
      return Collections.unmodifiableSet(attributes.keySet());
    }
  }

  /**
   * Returns the value of the specified attribute.
   *
   * @param name attribute name.
   * @return the value of the specified attribute, or <code>NULL</code> if the attribute does not exist.
   */
  public String getAttribute(String name) {
    if (attributes == null) {
      return null;
    } else {
      return attributes.get(name);
    }
  }

  /**
   * Sets an attribute.
   *
   * @param name attribute name.
   * @param value attribute value, it cannot be <code>NULL</code>.
   */
  public void setAttribute(String name, String value) {
    if (attributes == null) {
      attributes = new LinkedHashMap<>();
    }
    attributes.put(name, value);
  }

  /**
   * Deletes an attribute.
   *
   * @param name the attribute to delete.
   */
  public void deleteAttribute(String name) {
    if (attributes == null) {
      return;
    } else {
      attributes.remove(name);
    }
  }

  /**
   * Get all field attributes in an unmodifiable Map, or null if no attributes have been added
   *
   * @return all field attributes, or <code>NULL</code> if none exist
   */
  public Map<String, String> getAttributes() {
    if (attributes == null) {
      return null;
    } else {
      return Collections.unmodifiableMap(attributes);
    }
  }

  /**
   * Returns the string representation of the field.
   *
   * @return the string representation of the field.
   */
  @Override
  public String toString() {
    return type.toString(value);
  }

  /**
   * Returns the hashcode of the field.
   * <p/>
   * The hashcode is value based and it is computed in a deep fashion.
   * @return the hashcode of the field.
   */
  @Override
  public int hashCode() {
    return (value != null) ? value.hashCode() : 0;
  }

  /**
   * Compares if the field is equal to another field.
   * <p/>
   * The comparison is done in a deep fashion.
   * @return if the field is equal to another field.
   */
  @Override
  public boolean equals(Object obj) {
    boolean eq = false;
    if (obj != null && obj instanceof Field) {
      Field other = (Field) obj;
      if (type == other.type) {
        eq = (value == other.value) || type.equals(value, other.value);
      }
      if (attributes == null) {
        eq &= other.attributes == null;
      } else {
        eq &= attributes.equals(other.attributes);
      }
    }
    return eq;
  }

  /**
   * <p>
   * Returns a clone of the field.
   * </p>
   *
   * @return a clone of the field (deep copy).
   */
  @Override
  public Field clone() {
    return new Field(type, value, attributes);
  }

}
