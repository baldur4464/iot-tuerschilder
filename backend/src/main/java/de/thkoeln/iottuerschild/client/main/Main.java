package de.thkoeln.iottuerschild.client.main;

import de.thkoeln.iottuerschild.client.database.Database;
import de.thkoeln.iottuerschild.client.mqttnachricht.MQTTNachricht;
import de.thkoeln.iottuerschild.client.publisher.Publisher;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {
    public static void main (String[] Args){

        int auswahl = 0;
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

        Publisher pub = Publisher.getInstance();

        Database database = new Database();
        database.connect();


        boolean run = true;

        do {
            System.out.println("Testprogramm zum Versenden von Nachrichten");
            System.out.println("[1] Sende eine Testnachricht");
            System.out.println("[0] Beende das Programm");


            try {
                String input = br.readLine();
                auswahl = Integer.parseInt(input);
            } catch (Exception e) {
                e.printStackTrace();
            }


            switch (auswahl) {
                case 1:
                    MQTTNachricht nachricht = new MQTTNachricht("Test", "Raum/5", "12:00", "13:00", "5");
                    nachricht.nachrichtToJSON();
                    pub.sendNachricht(nachricht.getTopic(), nachricht.nachrichtToJSON().toString(), 0);
                    break;
                case 2:
                    System.out.println("Programm wird beendet");
                    run = false;
                    break;
                default:
                    System.out.println("ungültige Eingabe");
            }
        } while (run);
    }
}
