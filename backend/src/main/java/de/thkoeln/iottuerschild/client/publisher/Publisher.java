package de.thkoeln.iottuerschild.client.publisher;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class Publisher {

    private static Publisher instance;

    public static Publisher getInstance() {
        if(instance == null) {
            instance = new Publisher();
        }
        return instance;
    }

    private Publisher() {

    }

    private void configClient (String broker, String clientId) {

    }

    public void sendNachricht (String topic, String content, int qos) {


        try {
            MqttClient client = new MqttClient("tcp://127.0.0.1", "test", new MemoryPersistence());
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setCleanSession(false);
            connOpts.setKeepAliveInterval(15);
            connOpts.setConnectionTimeout(30);
            client.connect();
            MqttMessage message = new MqttMessage(content.getBytes());
            message.setQos(qos);
            message.setRetained(true);
            client.publish(topic, message);
            client.disconnect();
        } catch (MqttException me) {
            System.out.println("reason "+me.getReasonCode());
            System.out.println("msg "+me.getMessage());
            System.out.println("loc "+me.getLocalizedMessage());
            System.out.println("cause "+me.getCause());
            System.out.println("excep "+me);
            me.printStackTrace();
        }

    }

}
