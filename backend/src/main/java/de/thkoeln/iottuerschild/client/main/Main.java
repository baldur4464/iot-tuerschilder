package de.thkoeln.iottuerschild.client.main;

import de.thkoeln.iottuerschild.client.database.Database;
import de.thkoeln.iottuerschild.client.menu.Menu;
import de.thkoeln.iottuerschild.client.mqttnachricht.MQTTNachricht;
import de.thkoeln.iottuerschild.client.publisher.Publisher;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Main extends Thread {
    public static void main (String[] Args){

        Database database = new Database();
        database.connect();

        Menu menu = new Menu();
        Main main = new Main();

        main.start();
        menu.run();




    }

    public void run() {
        while(true){
            try {
                //System.out.println("Mainthread");
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
