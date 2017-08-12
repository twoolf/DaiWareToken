// Databricks notebook source
// MAGIC %md ### Infer, Serialize, Deserialize the Schema for the Meetup Datasets
// MAGIC Note: generateSchemaObjects needs to be called once

// COMMAND ----------

// MAGIC %run "./schemaTools"

// COMMAND ----------

// MAGIC %md ### Read in Meetup Data Using Serialized Schema

// COMMAND ----------

val schema = deserializeSchema(Source.fromFile("/dbfs/mnt/databricks-caryl/serSchem.txt").getLines.mkString)
val memSchema = deserializeSchema(Source.fromFile("/dbfs/mnt/databricks-caryl/serMemSchem.txt").getLines.mkString)
val evSchema = deserializeSchema(Source.fromFile("/dbfs/mnt/databricks-caryl/serEvSchem.txt").getLines.mkString)

val meetup = spark.read.schema(schema).json("/mnt/databricks-caryl/meetup/2017/*/*/*")
val members = spark.read.schema(memSchema).json("/mnt/databricks-caryl/meetupMemFirehose2017/*/*/*")
val events = spark.read.schema(evSchema).json("/mnt/databricks-caryl/meetupEvFirehose2017/*/*/*")

// COMMAND ----------

meetup.printSchema

// COMMAND ----------

val rsvps = meetup.withColumnRenamed("group", "rsvpGroup").withColumnRenamed("venue", "rsvpVenue")
val membersAndRSVPs = rsvps.join(members, $"member.member_id" === members.col("id"), "inner")
val joinedSet = membersAndRSVPs.join(events, $"event.event_id" === events.col("id"), "inner")

// COMMAND ----------

// MAGIC %md ### Transform Data for Model Training

// COMMAND ----------

val flat = joinedSet.select("rsvpGroup.group_topics", "response").withColumn("response", when(col("response").equalTo("yes"), "1").otherwise("0"))

// COMMAND ----------

val readyToTransform = flat.withColumn("group_topics", $"group_topics.topic_name".cast("string")).withColumn("responseNum",$"response".cast("double")).drop("response").withColumnRenamed("responseNum","response")

// COMMAND ----------

// MAGIC %md ### Create a Pipeline Containing Transformers and Estimator

// COMMAND ----------

import org.apache.spark.ml.feature.{Binarizer,Tokenizer, HashingTF}
import org.apache.spark.ml.classification.LogisticRegression
import org.apache.spark.ml.Pipeline

val bin: Binarizer = new Binarizer()
  .setInputCol("response")
  .setOutputCol("label")
  .setThreshold(0.5)
val tok = new Tokenizer()
  .setInputCol("group_topics")
  .setOutputCol("words")
val hashTF = new HashingTF()
  .setInputCol("words").setOutputCol("features").setNumFeatures(10000)
val lr = new LogisticRegression()
  .setMaxIter(10)
  .setRegParam(0.00001)
  .setElasticNetParam(0.1)
  .setThreshold(0.1)
val pipeline = new Pipeline()
  .setStages(Array(bin, tok, hashTF, lr))

// COMMAND ----------

val lrModel = pipeline.fit(readyToTransform)

// COMMAND ----------

import org.apache.spark.ml.{Pipeline, PipelineModel}
val pipeline = new Pipeline()
  .setStages(Array(cvm, mvm, yesBucketizer, waitBucketizer, feeBucketizer, groupCityIndexer, memberCityIndexer, labelIndexer, assembler, lr))

val lrModel = pipeline.fit(readyToTransform)

// COMMAND ----------

// MAGIC %md ### Save the Trained Model and Pipeline to Disk for Real-Time Scoring 

// COMMAND ----------

lrModel.write.overwrite().save("/mnt/databricks-caryl/meetup/model/member-rsvp-lr-model")

// COMMAND ----------

// MAGIC %md ###Save the Schema for the Model Training Data to Disk

// COMMAND ----------

persistSchema(readyToTransform, "/dbfs/mnt/databricks-caryl/simpleSchem.txt")