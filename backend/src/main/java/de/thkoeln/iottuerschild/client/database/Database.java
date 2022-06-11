package de.thkoeln.iottuerschild.client.database;

import java.sql.*;
import java.io.File;
import java.util.ArrayList;

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
                     "(RAUM_ID INTEGER PRIMARY KEY," +
                     "RAUM_NAME TEXT NOT NULL," +
                     "RAUM_TOPIC TEXT NOT NULL)";

            stmt.executeUpdate(sql);

            /*
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

           */


        } catch (SQLException e) {
            e.printStackTrace();
        }
        System.out.println("Neue Datenbank wird erstellt");
    }

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

    public ArrayList getRaeume () {

        Connection conn = this.connect();
        String sql = "SELECT * FROM RAUM";
        ArrayList<Raum> raumList = new ArrayList<>();

        try {
            Statement stmt = conn.createStatement();
            ResultSet rs = stmt.executeQuery(sql);
            System.out.println("Vor der Whileschleife");
            while (rs.next()){

                System.out.println(rs.getInt("RAUM_ID"));

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

}
