package de.thkoeln.iottuerschild.client.main;


import de.thkoeln.iottuerschild.client.database.Database;
import de.thkoeln.iottuerschild.client.menu.Menu;

public class Main extends Thread {

    private static Menu menu;
    private static Main main;


    public static void main (String[] Args){

        menu = new Menu();
        main = new Main();


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
