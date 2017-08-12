// Databricks notebook source
import org.apache.spark.sql.Dataset
import org.apache.spark.sql.types.{StructType, StructField, DataType}
import org.apache.spark.sql.functions._
import org.apache.spark.sql.catalyst.parser.LegacyTypeStringParser
import scala.util.{Try, Success, Failure}

/** Produce a Schema string from a Dataset */
def serializeSchema(ds: Dataset[_]): String = ds.schema.json

/** Produce a StructType schema object from a JSON string */
def deserializeSchema(json: String): StructType = {
    Try(DataType.fromJson(json)).getOrElse(LegacyTypeStringParser.parse(json)) match {
        case t: StructType => t
        case _ => throw new RuntimeException(s"Failed parsing StructType: $json")
    }
}

// COMMAND ----------

def persistSchema(f:String, p:String): Unit = {
  import java.io.{PrintWriter, File}
  val data = spark.read.json(f)
  val schema = serializeSchema(data)
  val pw = new PrintWriter(new File(p))
  pw.write(schema)
  pw.close
}

// COMMAND ----------

def generateSchemaObjects(): Unit = {
  persistSchema("/mnt/databricks-caryl/meetup/2017/05/10/19*", "/dbfs/mnt/databricks-caryl/serSchem.txt")
  persistSchema("/mnt/databricks-caryl/meetupMemFirehose2017/05/29/*", "/dbfs/mnt/databricks-caryl/serMemSchem.txt")
  persistSchema("/mnt/databricks-caryl/meetupEvFirehose2017/06/01/*", "/dbfs/mnt/databricks-caryl/serEvSchem.txt")
}