// Databricks notebook source
// MAGIC %md ![AWS Kinesis Firehose](http://i.imgur.com/rgpXaD5.png)

// COMMAND ----------

// MAGIC %md ### Load the Schema and Data

// COMMAND ----------

// MAGIC %run "./schemaTools"

// COMMAND ----------

val inputPath = "/mnt/databricks-caryl/meetup/2017/*/*/*"

// COMMAND ----------

import scala.io.Source
val schema = deserializeSchema(Source.fromFile("/dbfs/mnt/databricks-caryl/serSchem.txt").getLines.mkString)


// COMMAND ----------

// MAGIC %md ### Read the Stream and Calculate the RSVP Counts by Country

// COMMAND ----------

val streamReader = spark.readStream.schema(schema).option("maxFilesPerTrigger", 10).json(inputPath)
val streamingCounts = streamReader.groupBy($"group.group_country").count()
val inMemCounter = streamingCounts.writeStream.format("memory").queryName("countryCount").outputMode("complete").start()

// COMMAND ----------

// MAGIC %sql select * from countryCount where group_country <> 'us'

// COMMAND ----------

inMemCounter.stop()
spark.catalog.dropTempView("inMemCounter")