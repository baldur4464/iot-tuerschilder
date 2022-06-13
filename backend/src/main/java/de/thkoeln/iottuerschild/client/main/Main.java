package de.thkoeln.iottuerschild.client.main;


import de.thkoeln.iottuerschild.client.calendar.CalendarAPI;
import de.thkoeln.iottuerschild.client.database.Database;
import de.thkoeln.iottuerschild.client.menu.Menu;

import javax.xml.crypto.Data;

public class Main extends Thread {

    private static Menu menu;
    private static Main main;
    private static CalendarAPI cAPI;


    public static void main (String[] Args){


        Thread menuThread = new Thread(() -> {
            menu = new Menu();
            menu.run();
        });

        Thread calendarAPIThread = new Thread(() -> {
            cAPI = new CalendarAPI();
            cAPI.run();
        });

        menuThread.start();
        calendarAPIThread.start();

    }

    public void run() {
        while(true){

        }
    }
}
