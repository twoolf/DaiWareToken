## Introduction
With IBM® Streaming Analytics for Bluemix™, you can perform real-time analysis on data in motion as part of your Bluemix application. The Streaming Analytics service is powered by IBM InfoSphere® Streams, which is an advanced analytic platform that custom applications use to quickly ingest, analyze, and correlate information as it is produced by real-time data sources. InfoSphere Streams can handle very high data rates and perform its analysis with predictable low-latency, so your application can operate at the speed of data.

In this project, we show how to use the Streaming Analytics service to analyze the events published by IoT devices, on the IBM Watson IoT Platform. IBM Watson Internet of Things Platform is a fully managed, cloud-hosted service that makes it simple to derive value from Internet of Things (IoT) devices. The platform provides simple, but powerful application access to IoT devices and data. 

### Architecture

The following diagram shows the components involved in the integration.

![Alt text](./architecture.PNG?raw=true "High Level Architecture")

As shown, the IoT devices “Washing Machines” will publish various sensor events to IBM Watson IoT Platform. In absence of an actual device, we have provided a simulator which keep pumping in the events. Following are the different events that the Washer will publish to Watson IoT Platform.

* **Fluid events** having flowrate, hardness, fluidlevel and temperature.
* **Voltage events** having voltage and frequency.
* **Mechanical events** having the drum speed.

The IBM Streaming Analytics service will consume these events in real-time and perform the following operation,

* **View Events** – Displays the events in the Streaming Analytics dashboard.
* **Threshold Based Anomaly detection** – Detects the threshold breach and sends a command back to the device to take corrective actions.
* **Anomaly detection** – Detects the anomaly by comparing the current set of values with previous set and reports to device using the command.
* **Statistics calculation over a period of time** – Aggregates the events for a specified time, computes the statistics and sends back to device as a command.

----

### Instruction to Run
-------------------

The samples are built using the [InfoSphere Streams Studio integrated development environment (IDE)](https://www.ibm.com/support/knowledgecenter/SSCRJU_4.1.0/com.ibm.streams.qse.doc/doc/installtrial-container.html?lang=en) and  toolkits that extend the functionality.

Also, [this recipe](https://developer.ibm.com/recipes/tutorials/integrate-ibm-streaming-analytics-service-with-watson-iot-platform/) showcases how to run the project step by step to perform the realtime Streaming Analytics on the events published on IBM Watson IoT Platform.

----

### License
-----------------------

The library is shipped with Eclipse Public License and refer to the [License file] (https://github.com/ibm-watson-iot/streaming-analytic-samples/blob/master/LICENSE) for more information about the licensing.

----
