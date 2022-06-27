package de.thkoeln.iottuerschild.client.database;

import java.sql.*;
import java.io.File;
import java.util.ArrayList;


/**
 * Datenbank-Modell für das Backend um Räume zu speichern
 * @author Patrick Schmidt
 * @version 1.0
 * @since 15.06.2022
 */
public class Database {
    /**
     * Erstellt eine Verbindung mit der Datenbank her und erstellt eine neue Datenbank, wenn diese nicht
     * existiert.
     * @return Connection gibt die Verbindung der Datenbank zurück
     */
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

    /**
     * Erstellt eine Datenbank mit einer Tabelle für Raum
     */
    public void createNewDatabase () {
        String url = "jdbc:sqlite:sqlite/db/test.db";
        try {
            Connection conn = DriverManager.getConnection(url);
            Statement stmt = conn.createStatement();

            //SQL Anweisung für Tabelle Raum mit RAUM_ID als PK, RAUM_NAME und RAUM_TOPIC
            String sql= "CREATE TABLE RAUM " +
                     "(RAUM_ID INTEGER PRIMARY KEY," +
                     "RAUM_NAME TEXT NOT NULL," +
                     "RAUM_TOPIC TEXT NOT NULL)";

            stmt.executeUpdate(sql);
            stmt.close();
            conn.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }
        System.out.println("Neue Datenbank wird erstellt");
    }

    /**
     * Diese Funktion erstellt einen neuen Eintrag in der Datenbank für raum
     * @param raumName Name des Raums
     * @param raumTopic Topic URI des Raums
     */
    public void erstelleRaum (String raumName, String raumTopic) {

        Connection conn = this.connect();

        String sql = "INSERT INTO RAUM (RAUM_NAME, RAUM_TOPIC) VALUES(?,?)";

        try {
            PreparedStatement pstmt = conn.prepareStatement(sql);
            pstmt.setString(1, raumName);
            pstmt.setString(2, raumTopic);
            pstmt.executeUpdate();
            pstmt.close();
            conn.close();
        } catch (SQLException throwables) {
            throwables.printStackTrace();
        }

    }

    /**
     * Löscht einen Raum mit der ID des zu löschenden Raum
     * @param raumId ID des Raums
     */
    public void loescheRaum (int raumId) {
        Connection conn = this.connect();

        String sql = "DELETE FROM RAUM WHERE RAUM_ID = ?";

        try {
            PreparedStatement pstmt = conn.prepareStatement(sql);
            pstmt.setInt(1, raumId);
            pstmt.executeUpdate();
            pstmt.close();
            conn.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Gibt alle verfügbaren Räume in der Datenbank aus
     * @return ArrayList Eine Liste von Räumen
     */
    public ArrayList getRaeume () {

        Connection conn = this.connect();
        String sql = "SELECT * FROM RAUM";
        ArrayList<Raum> raumList = new ArrayList<>();

        try {
            Statement stmt = conn.createStatement();
            ResultSet rs = stmt.executeQuery(sql);

            while (rs.next()){

                Raum raum = new Raum(
                        rs.getInt("RAUM_ID"),
                        rs.getString("RAUM_NAME"),
                        rs.getString("RAUM_TOPIC"));
                raumList.add(raum);
            }

            rs.close();
            conn.close();
            return raumList;

        } catch(Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * Gibt einen einzelnen Raum über den Namen aus
     * @param raumName Name des Raums
     * @return Raum-Objekt zugehärig des Raumnmaen
     */
    public Raum getRaumByName(String raumName) {
        Raum raum = null;
        Connection conn = this.connect();
        String sql = "SELECT * FROM RAUM WHERE RAUM_NAME = ?";


        try {
            PreparedStatement pstmt = conn.prepareStatement(sql);
            pstmt.setString(1, raumName);
            ResultSet rs = pstmt.executeQuery();

            if(rs.next()) {
                raum = new Raum(
                        rs.getInt("RAUM_ID"),
                        rs.getString("RAUM_NAME"),
                        rs.getString("RAUM_TOPIC"));

            }

            conn.close();
            return raum;

        } catch(Exception e) {
            e.printStackTrace();
            return null;
        }
    }

}
