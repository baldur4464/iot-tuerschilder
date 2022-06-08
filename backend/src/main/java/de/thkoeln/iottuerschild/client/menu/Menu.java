package de.thkoeln.iottuerschild.client.menu;

import de.thkoeln.iottuerschild.client.mqttnachricht.MQTTNachricht;
import de.thkoeln.iottuerschild.client.publisher.Publisher;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Menu implements Runnable{
    @Override
    public void run() {

        boolean run = true;
        int auswahl = 0;

        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        Publisher pub = Publisher.getInstance();



        do {
            System.out.println("Testprogramm zum Versenden von Nachrichten");
            System.out.println("[1] Sende eine Testnachricht");
            System.out.println("[0] Beende das Programm");
            System.out.print("Eingabe: ");


            try {
                String input = br.readLine();
                auswahl = Integer.parseInt(input);
            } catch (Exception e) {
                e.printStackTrace();
            }


            switch (auswahl) {
                case 1:
                    MQTTNachricht nachricht = new MQTTNachricht("Test", "raum/5", "12:00", "13:00", "5");
                    pub.sendNachricht(nachricht.getTopic(), nachricht.nachrichtToJSON().toString(), 0);
                    break;
                case 0:
                    System.out.println("Programm wird beendet");
                    System.exit(0);
                    run = false;
                    break;
                default:
                    System.out.println("ungültige Eingabe");
            }
        } while (run);
    }
}
