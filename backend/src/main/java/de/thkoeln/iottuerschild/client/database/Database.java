package de.thkoeln.iottuerschild.client.database;

import java.sql.Connection;
import java.sql.DriverManager;
import java.io.File;
import java.sql.SQLException;
import java.sql.Statement;


public class Database {

    public Connection connect () {

        Connection conn = null;

        File existDB = new File("sqlite/db/test.db");

        if(existDB.exists()) {
            try{
                String url = "jdbc:sqlite:sqlite/db/test.db";
                conn = DriverManager.getConnection(url);
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            createNewDatabase();
        }

        return conn;
    }

    private void createNewDatabase () {
        String url = "jdbc:sqlite:sqlite/db/test.db";
        try {
            Connection conn = DriverManager.getConnection(url);
            Statement stmt = conn.createStatement();
            String sql = "";

             sql= "CREATE TABLE RAUM " +
                     "(RAUM_ID INT PRIMARY KEY NOT NULL," +
                     "RAUM_NAME TEXT NOT NULL," +
                     "RAUM_TOPIC TEXT NOT NULL)";

            stmt.executeUpdate(sql);

            sql = "CREATE TABLE BUCHUNG " +
                    "(BUCHUNG_ID INT PRIMARY KEY NOT NULL," +
                    "RAUM_ID INT NOT NULL," +
                    "VERANTWORTLICHER TEXT NOT NULL," +
                    "UHRZEIT_START DATETIME NOT NULL," +
                    "UHRZEIT_ENDE DATETIME NOT NULL," +
                    "FOREIGN KEY (RAUM_ID) " +
                    "REFERENCES RAUM (RAUM_ID) " +
                    "ON DELETE CASCADE " +
                    "ON UPDATE NO ACTION)";

            System.out.println(sql);

            stmt.execute(sql);


        } catch (SQLException e) {
            e.printStackTrace();
        }
        System.out.println("Neue Datenbank wird erstellt");
    }

}
