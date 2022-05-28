package de.thkoeln.iottuerschild.client.main;

import de.thkoeln.iottuerschild.client.database.Database;
import de.thkoeln.iottuerschild.client.mqttnachricht.MQTTNachricht;
import de.thkoeln.iottuerschild.client.publisher.Publisher;
import org.json.JSONObject;

public class Main {
    public static void main (String[] Args) {
        System.out.println("Hello World");

        MQTTNachricht nachricht = new MQTTNachricht("Test", "Raum/5", "12:00", "13:00", "5");
        nachricht.nachrichtToJSON();

        Publisher.sendNachricht("tcp://127.0.0.1:1883",nachricht.getTopic(), nachricht.nachrichtToJSON().toString(), 0, "Test");

        Database database = new Database();
        database.connect();

    }
}
