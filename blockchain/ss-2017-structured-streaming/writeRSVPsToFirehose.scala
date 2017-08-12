// Databricks notebook source
// MAGIC %md #####Load AWS Credentials 

// COMMAND ----------

// MAGIC %run "./AWSCredentials"

// COMMAND ----------

val aws = new AWSCredentials()

// COMMAND ----------

aws.createSession()

// COMMAND ----------

// MAGIC %md #####Retrieve RSVP Requests and Write to a Kinesis Firehose Client

// COMMAND ----------

// MAGIC %sh echo "
// MAGIC import requests
// MAGIC import boto3
// MAGIC import json
// MAGIC 
// MAGIC r = requests.get('http://stream.meetup.com/2/rsvps', stream = True)
// MAGIC kinesis = boto3.client('firehose')
// MAGIC 
// MAGIC rsvps = []
// MAGIC count = 0
// MAGIC for line in r.iter_lines():
// MAGIC     if line:
// MAGIC         rsvp = line.decode('utf-8')
// MAGIC         rsvps.append({'Data': rsvp + '\n'})
// MAGIC         count += 1
// MAGIC         if count == 100:
// MAGIC             kinesis.put_record_batch(DeliveryStreamName='meetup', Records=rsvps)
// MAGIC             print('100RecordsPosted')
// MAGIC             count = 0
// MAGIC             rsvps = []
// MAGIC 
// MAGIC 
// MAGIC " > meetup.py

// COMMAND ----------

// MAGIC %md ##### Install Python Packages on Driver

// COMMAND ----------

// MAGIC %sh pip install awscli
// MAGIC pip install requests
// MAGIC pip install boto3

// COMMAND ----------

// MAGIC %md ##### Trigger the Kinesis Producer

// COMMAND ----------

// MAGIC %sh python meetup.py