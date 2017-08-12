// Databricks notebook source
// MAGIC %md #####Load AWS Credentials 

// COMMAND ----------

// MAGIC %run "./AWSCredentials"

// COMMAND ----------

val aws = new AWSCredentials()

// COMMAND ----------

aws.createSession

// COMMAND ----------

// MAGIC %md #####Retrieve Events and Write to a Kinesis Firehose Client
// MAGIC Note: You will be rate-limited by the API

// COMMAND ----------

// MAGIC %sh echo "
// MAGIC import json
// MAGIC import requests
// MAGIC import csv
// MAGIC import boto3
// MAGIC import time
// MAGIC 
// MAGIC kinesis = boto3.client('firehose')
// MAGIC 
// MAGIC events = []
// MAGIC count = 0
// MAGIC 
// MAGIC for file in range (5,9):
// MAGIC     fileName = '/dbfs/mnt/databricks-caryl/meetupEvents/part-0000' + str(file) + '-26701a6b-4a2c-444a-9be2-02dbbc0f0d23-c000.csv'
// MAGIC     with open(fileName) as csvfile:
// MAGIC         reader = csv.reader(csvfile, delimiter=',', quoting=csv.QUOTE_NONE)
// MAGIC         for row in reader:
// MAGIC             urlname = ''.join(row[0])
// MAGIC             id = ''.join(row[1])
// MAGIC             myurl='http://api.meetup.com/' + urlname + '/events/'+ id + '?key=2a7f25635714485344173e714d4a3b7c'
// MAGIC             r = requests.get(myurl)
// MAGIC             for line in r.iter_lines():
// MAGIC                 if line:
// MAGIC                     eventInfo = line.decode('utf-8')
// MAGIC                     events.append({'Data': eventInfo + '\n'})
// MAGIC                     count += 1
// MAGIC                     if count == 28:
// MAGIC                         kinesis.put_record_batch(DeliveryStreamName='meetupEv', Records = events)
// MAGIC                         print('28 Records Posted' + str(file))
// MAGIC                         time.sleep(10)
// MAGIC                         count = 0
// MAGIC                         events = []     
// MAGIC 
// MAGIC " > meetupEv.py

// COMMAND ----------

// MAGIC %md ##### Install Python Packages on Driver

// COMMAND ----------

// MAGIC %sh 
// MAGIC pip install --upgrade pip
// MAGIC pip install awscli
// MAGIC pip install requests
// MAGIC pip install boto3

// COMMAND ----------

// MAGIC %md ##### Trigger the Kinesis Producer

// COMMAND ----------

// MAGIC %sh python meetupEv.py