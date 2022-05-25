package de.thkoeln.iottuerschild.client.main;

import de.thkoeln.iottuerschild.client.database.Database;

public class Main {
    public static void main (String[] Args) {
        System.out.println("Hello World");

        Database database = new Database();

        database.connect();

    }
}
