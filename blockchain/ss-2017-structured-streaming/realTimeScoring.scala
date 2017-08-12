// Databricks notebook source
// MAGIC %md ### Retrieve the Training Data Schema

// COMMAND ----------

// MAGIC %run "./schemaTools"

// COMMAND ----------

import scala.io.Source
val schema = deserializeSchema(Source.fromFile("/dbfs/mnt/databricks-caryl/simpleSchem.txt").getLines.mkString)

// COMMAND ----------

// MAGIC %md ### Load the Serialized Model Pipeline

// COMMAND ----------

import org.apache.spark.ml.{Pipeline, PipelineModel}
val lrModel = PipelineModel.load("/mnt/databricks-caryl/meetup/model/member-rsvp-lr-model")

// COMMAND ----------

import org.apache.spark.sql.functions._

val scoringDF = 
  spark
    .readStream                       
    .schema(schema)
    .option("maxFilesPerTrigger", 10) 
    .parquet("/mnt/databricks-caryl/meetup/simpleJoinedStream")


// COMMAND ----------

// MAGIC %md ### Transform Streaming DataFrame with Loaded Pipeline

// COMMAND ----------

val scoredStream = lrModel.transform(scoringDF)

// COMMAND ----------

display(scoredStream.select("label", "features", "prediction"))