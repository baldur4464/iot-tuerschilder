package de.thkoeln.iottuerschild.client.publisher;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import io.github.cdimascio.dotenv.*;

public class Publisher {

    private static Publisher instance;
    private MqttClient client;
    private final MemoryPersistence persistence;
    private Dotenv dotenv;

    public static Publisher getInstance() {
        if(instance == null) {
            instance = new Publisher();
        }
        return instance;
    }

    private Publisher() {
        dotenv = Dotenv.load();
        persistence = new MemoryPersistence();
        connect(dotenv.get("BROKER"), dotenv.get("CLIENT_ID"));
    }

    private void connect (String broker, String clientId) {
        try {
            client = new MqttClient(broker, clientId, persistence);
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setCleanSession(false);
            connOpts.setKeepAliveInterval(15);
            connOpts.setConnectionTimeout(30);
            client.connect();
        } catch (MqttException e) {
            e.printStackTrace();
        }

    }

    public void sendNachricht (String topic, String content, int qos) {

        try {
            MqttMessage message = new MqttMessage(content.getBytes());
            message.setQos(qos);
            message.setRetained(true);
            client.publish(topic, message);
        } catch (MqttException me) {
            me.printStackTrace();
        }
    }
}
