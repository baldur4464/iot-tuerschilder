package de.thkoeln.iottuerschild.client.publisher;

import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import io.github.cdimascio.dotenv.*;

/**
 * Diese Klasse publisht eine Nachricht den MQTT-Broker
 * @author Patrick Schmidt
 * @version 1.0
 * @since 15.06.2022
 */
public class Publisher {

    private static Publisher instance;
    private MqttClient client;
    private final MemoryPersistence persistence;
    private Dotenv dotenv;

    /**
     * Singleton der Publisherklasse
     * @return Publisher
     */
    public static Publisher getInstance() {
        if(instance == null) {
            instance = new Publisher();
        }
        return instance;
    }

    /**
     * Bei Aufruf der Klasse lädt die KLasse seine Environment-Daten und connectet sich mit dem Broker
     */
    private Publisher() {
        dotenv = Dotenv.load();
        persistence = new MemoryPersistence();
        connect(dotenv.get("BROKER"), dotenv.get("CLIENT_ID"));
    }

    /**
     * Verbindet den Publisher mit dem Broker
     * @param broker
     * @param clientId
     */
    private void connect (String broker, String clientId) {
        try {
            client = new MqttClient(broker, clientId, persistence);
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setCleanSession(false); //Keine Cleane Session (Daten bleiben erhalten)
            connOpts.setKeepAliveInterval(15);//KeepAliveInterval damit es zu keinem Verbindungsabbruch kommt
            connOpts.setConnectionTimeout(30);//Setzt einen Timeout sobald keine Verbindungen mehr stattfinden konnte
            client.connect();
        } catch (MqttException e) {
            e.printStackTrace();
        }

    }

    /**
     * Sendet eine Nachricht an ein bestimmtes Topic mit einem bestimmten content mit einem bestimmtwn qos (quality of service)
     * @param topic
     * @param content
     * @param qos
     */
    public void sendNachricht (String topic, String content, int qos) {

        try {
            MqttMessage message = new MqttMessage(content.getBytes());
            message.setQos(qos);
            message.setRetained(true);//Lässt eine Nachricht beim MQTT bestehen, bis diese Aufgerufen wurde oder durch eine neue ersetzt wurde
            client.publish(topic, message);
        } catch (MqttException me) {
            me.printStackTrace();
        }
    }
}
