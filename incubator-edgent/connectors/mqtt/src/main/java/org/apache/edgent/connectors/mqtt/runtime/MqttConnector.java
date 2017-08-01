/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

package org.apache.edgent.connectors.mqtt.runtime;

import java.util.concurrent.TimeUnit;

import org.apache.edgent.connectors.mqtt.MqttConfig;
import org.apache.edgent.connectors.runtime.Connector;
import org.apache.edgent.function.Supplier;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttClientPersistence;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * A connector to an MQTT server.
 */
public class MqttConnector extends Connector<MqttClient> {
    private static final long serialVersionUID = 1L;
    private static final Logger logger = LoggerFactory.getLogger(MqttConnector.class);
    private String id;
    private final String clientId;
    private final Supplier<MqttConfig> configFn;
    private volatile MqttSubscriber<?> subscriber;

    private class Callback implements MqttCallback {

        @Override
        public void connectionLost(Throwable t) {
            MqttConnector.this.connectionLost(t);
        }

        @Override
        public void deliveryComplete(IMqttDeliveryToken arg0) {
            // TODO Auto-generated method stub
        }

        @Override
        public void messageArrived(String topic, MqttMessage message)
                throws Exception {
            // N.B. MQTT will close the connection if this unwinds
            try {
                logger.trace("{} received topic:{} qos:{} isRetained:{} {} bytes",
                            id(), topic, message.getQos(), message.isRetained(),
                            message.getPayload().length);
                notIdle();
                subscriber.messageArrived(topic, message);
            }
            catch (Exception e) {
                logger.error("{} messageArrived handling failed", id(), e);
            }
        }
        
    }

    /**
     * Create a new connector.
     * 
     * @param config connector configuration.
     */
    public MqttConnector(Supplier<MqttConfig> config) {
        this.configFn = config;
        
        String cid = configFn.get().getClientId();
        if (cid == null)
            cid = MqttClient.generateClientId();
        clientId = cid;
    }
    @Override
    public Logger getLogger() {
        return logger;
    }
    
    void setSubscriber(MqttSubscriber<?> subscriber) {
        this.subscriber = subscriber;
    }

    @Override
    protected synchronized MqttClient doConnect(MqttClient client) throws MqttException {
        MqttConfig config = configFn.get();
        
        if (client == null)
            client = newClient(config);
        
        if (client.isConnected())
            return client;
        
        MqttConnectOptions options = (MqttConnectOptions) config.options();
        
        logger.info("{} cleanSession:{} userName:{} password:{} idleTimeout:{} idleReconnectTimeout:{} cnTimeout:{} keepalive:{} serverURIs:{} willDst:{} willMsg:{}",
                id(),
                options.isCleanSession(),
                options.getUserName(),
                options.getPassword() == null ? null : "*****",
                config.getIdleTimeout(),
                config.getSubscriberIdleReconnectInterval(),
                options.getConnectionTimeout(),
                options.getKeepAliveInterval(),
                options.getServerURIs(),
                options.getWillDestination(),
                options.getWillMessage()
                );
        
        client.connect(options);

        setIdleTimeout(config.getIdleTimeout(), TimeUnit.SECONDS);
        
        MqttSubscriber<?> sub = subscriber;
        if (sub != null) {
            setIdleReconnectInterval(config.getSubscriberIdleReconnectInterval());
            sub.connected(client);
        }

        return client;
    }
    
    private MqttClient newClient(MqttConfig config) throws MqttException {
        String url = config.getServerURLs()[0];
        MqttClientPersistence persistence = config.getPersistence();
        if (persistence == null)
            persistence = new MemoryPersistence();
        long actionTimeToWaitMillis = config.getActionTimeToWaitMillis(); 
        
        logger.info("{} server:{} clientId:{} actionTimeToWait:{} persistence:{}",
                id(), url, clientId, actionTimeToWaitMillis, persistence);
            
        MqttClient client = new MqttClient(url, clientId, persistence);
        client.setTimeToWait(actionTimeToWaitMillis);
        client.setCallback(new Callback());
        return client;
    }

    @Override
    protected synchronized void doDisconnect(MqttClient client) throws Exception {
        if (client.isConnected())
            client.disconnect();
    }

    @Override
    public void doClose(MqttClient client) throws Exception {
        try {
            doDisconnect(client);
        } finally {
            client.close();
        }
    }
    
    @Override
    protected String id() {
        if (id == null) {
            // include our short object Id
            id = "MQTT " + toString().substring(toString().indexOf('@') + 1)
                    + " " + clientId;
        }
        return id;
    }
}
