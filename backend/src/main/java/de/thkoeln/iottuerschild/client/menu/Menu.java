package de.thkoeln.iottuerschild.client.menu;

import de.thkoeln.iottuerschild.client.database.Database;
import de.thkoeln.iottuerschild.client.database.Raum;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;


public class Menu implements Runnable{
    @Override
    public void run() {

        boolean run = true;
        int auswahl = 0;

        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));

        do {
            System.out.println("Testprogramm zum Versenden von Nachrichten");
            System.out.println("[1] Neuen Raum erstellen");
            System.out.println("[2] Alle Räume anzeigen");
            System.out.println("[3] Einen Raum löschen");
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
                    erstelleRaum (br);
                    break;
                case 2:
                    zeigeRaeume();
                    break;
                case 3:
                    loescheRaum(br);
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

    private void erstelleRaum(BufferedReader br) {
        String raumName;
        String raumTopic;

        System.out.println("Neuer Raum erstellen...");

        System.out.print("Geben Sie einen Raumnamen ein: ");
        try {
            raumName = br.readLine();
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

        System.out.print("Geben Sie einen Topic ein auf dem der Raum publiziert: ");
        try {
            raumTopic = br.readLine();
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

        Database db = new Database();
        db.erstelleRaum(raumName, raumTopic);
    }

    private void zeigeRaeume() {
        Database db = new Database();
        ArrayList<Raum> raumList = db.getRaeume();

        for(Raum raum: raumList) {
            System.out.println(
                    "ID: "+ raum.getRaumId()+"\t\t"+
                    "Raum Name: "+raum.getRaumName()+"\t\t"+
                    "Raum Topic:"+raum.getRaumTopic());
        }
    }

    private void loescheRaum(BufferedReader br) {
        Database db = new Database();
        ArrayList<Raum> raumList = db.getRaeume();
        int zaehler = 1;
        int eingabeInt = 0;

        System.out.println("Welchen Raum wollen Sie löschen? ");
        System.out.println("[0] Zurück zum Hauptmenü");

        for(Raum raum: raumList) {
            System.out.println("["+zaehler+"] " +"Raumname: " + raum.getRaumName());
            zaehler++;
        }

        System.out.print("Eingabe: ");
        try {
            String eingabe = br.readLine();
            eingabeInt = Integer.parseInt(eingabe);
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        if(eingabeInt == 0) {
            return;
        }

        if(eingabeInt > raumList.size()){
            System.out.println("Ungültige Eingabe. Versuchen Sie es erneut");
            return;
        }
        db.loescheRaum(raumList.get(eingabeInt-1).getRaumId());
        return;
    }
}
