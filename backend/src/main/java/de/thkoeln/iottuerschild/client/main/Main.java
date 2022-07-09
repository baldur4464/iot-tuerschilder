package de.thkoeln.iottuerschild.client.main;


import de.thkoeln.iottuerschild.client.calendar.CalendarAPI;
import de.thkoeln.iottuerschild.client.database.Database;
import de.thkoeln.iottuerschild.client.menu.Menu;

import java.sql.SQLException;

/**
 * Das Programm soll über die GoogleAPI aus dem Google Calendar Events holen, die Daten verarbeiten an den Broker
 * weiterleiten.
 *
 * @author Patrick Schmidt
 * @version 1.0
 * @since 15.06.2022
 */

public class Main{

    private static Menu menu;
    private static CalendarAPI cAPI;

    /**
     *
     * Mainklasse erstellt die Datenbank, falls keine exisiert und öffnet 2 Threads für Konsole und
     * die CalendarAPI
     *
     * @param Args Argumente, die beim Programmstart abgearbeitet werden können
     * @throws SQLException Falls die Datenbank nicht erstellt werden kann
     */
    public static void main (String[] Args) throws SQLException {

        Database db = new Database();
        db.createNewDatabase();


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
}
