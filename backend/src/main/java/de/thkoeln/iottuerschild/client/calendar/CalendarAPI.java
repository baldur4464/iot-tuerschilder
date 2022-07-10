package de.thkoeln.iottuerschild.client.calendar;

import com.google.api.client.auth.oauth2.Credential;
import com.google.api.client.extensions.java6.auth.oauth2.AuthorizationCodeInstalledApp;
import com.google.api.client.extensions.jetty.auth.oauth2.LocalServerReceiver;
import com.google.api.client.googleapis.auth.oauth2.GoogleAuthorizationCodeFlow;
import com.google.api.client.googleapis.auth.oauth2.GoogleClientSecrets;
import com.google.api.client.googleapis.javanet.GoogleNetHttpTransport;
import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.JsonFactory;
import com.google.api.client.json.gson.GsonFactory;
import com.google.api.client.util.DateTime;
import com.google.api.client.util.store.FileDataStoreFactory;
import com.google.api.services.calendar.Calendar;
import com.google.api.services.calendar.CalendarScopes;
import com.google.api.services.calendar.model.Event;
import com.google.api.services.calendar.model.Events;
import de.thkoeln.iottuerschild.client.database.Database;
import de.thkoeln.iottuerschild.client.database.Raum;
import de.thkoeln.iottuerschild.client.mqttnachricht.MQTTNachricht;
import de.thkoeln.iottuerschild.client.mqttnachricht.Nachricht;
import de.thkoeln.iottuerschild.client.publisher.Publisher;
import io.github.cdimascio.dotenv.Dotenv;
import org.checkerframework.checker.units.qual.A;
import org.json.JSONObject;


import java.io.*;
import java.security.GeneralSecurityException;
import java.text.SimpleDateFormat;
import java.util.*;

/**
 * Schnittstelle zwischen der Google CalendarAPI und dem Publisher. Das Programm sucht automatisch nach
 * Events, sotiert sie nach Räumen, erstellt MQTTNachricht und gibt die Nachrichten den den Publisher weiter.
 * Diese Schnittstelle kann auch durch andere Schnittstellen ersetzt werden.
 *
 * @author Patrick Schmidt
 * @version 1.0
 * @since 15.06.2022
 */
public class CalendarAPI implements Runnable{
    /** Application name. */
    private static final String APPLICATION_NAME = "Google Calendar API Java Quickstart";
    /** Global instance of the JSON factory. */
    private static final JsonFactory JSON_FACTORY = GsonFactory.getDefaultInstance();
    /** Directory to store authorization tokens for this application. */
    private static final String TOKENS_DIRECTORY_PATH = "tokens";

    /** Umgebungsvariablen in .env */
    private Dotenv dotenv;

    /**
     * Global instance of the scopes required by this quickstart.
     * If modifying these scopes, delete your previously saved tokens/ folder.
     */
    private static final List<String> SCOPES = Collections.singletonList(CalendarScopes.CALENDAR_READONLY);
    private static final String CREDENTIALS_FILE_PATH = "/credentials.json";

    public CalendarAPI () {
        dotenv = Dotenv.load();
    }

    /**
     * Creates an authorized Credential object.
     * @param HTTP_TRANSPORT The network HTTP Transport.
     * @return An authorized Credential object.
     * @throws IOException If the credentials.json file cannot be found.
     */
    private static Credential getCredentials(final NetHttpTransport HTTP_TRANSPORT) throws IOException {
        // Load client secrets.
        InputStream in = CalendarAPI.class.getResourceAsStream(CREDENTIALS_FILE_PATH);
        if (in == null) {
            throw new FileNotFoundException("Resource not found: " + CREDENTIALS_FILE_PATH);
        }
        GoogleClientSecrets clientSecrets = GoogleClientSecrets.load(JSON_FACTORY, new InputStreamReader(in));

        // Build flow and trigger user authorization request.
        GoogleAuthorizationCodeFlow flow = new GoogleAuthorizationCodeFlow.Builder(
                HTTP_TRANSPORT, JSON_FACTORY, clientSecrets, SCOPES)
                .setDataStoreFactory(new FileDataStoreFactory(new java.io.File(TOKENS_DIRECTORY_PATH)))
                .setAccessType("offline")
                .build();
        LocalServerReceiver receiver = new LocalServerReceiver.Builder().setPort(8888).build();
        Credential credential = new AuthorizationCodeInstalledApp(flow, receiver).authorize("user");
        //returns an authorized Credential object.
        return credential;
    }

    /*
     * Testmain um die Funktionsfähigkeit dieser Komponente zu Testen.
     */
    /*public static void main(String... args) throws IOException, GeneralSecurityException {
        // Build a new authorized API client service.

        CalendarAPI api = new CalendarAPI();
        api.sendMqttNachricht();
        System.exit(1);
    }*/

    /**
     * Ruft eine Liste von Events über die Google API ab.
     *
     * @return Liste von Events die im Kalender eingetragen sind.
     */
    public List getEvents () {
        try {

            //Authentifizierung
            final NetHttpTransport HTTP_TRANSPORT = GoogleNetHttpTransport.newTrustedTransport();
            Calendar service = new Calendar.Builder(HTTP_TRANSPORT, JSON_FACTORY, getCredentials(HTTP_TRANSPORT))
                    .setApplicationName(APPLICATION_NAME)
                    .build();


            Date nowDate = new Date(System.currentTimeMillis());

            //Ersellt eine Maximale Abfragezeit von x Stunden ab dem Abfragezeitraum
            java.util.Calendar cal = java.util.Calendar.getInstance();
            cal.setTime(nowDate);
            cal.add(java.util.Calendar.HOUR_OF_DAY, 12);
            Date maxNow = cal.getTime();

            DateTime now = new DateTime(nowDate);
            DateTime maxDate = new DateTime(maxNow);

            /*
             * Parameter: die CalendarId
             * Abruf der Events mit folgenden Settings
             * Mindestzeitraum: Das jetztige Datum und Uhrzeit
             * Maximalzeitraum: Das jetztige Datum und Uhrzeit + x Stunden
             * Sortiert nach: Startdatum
             * Single Events: Wahr
             */
            Events events = service.events().list(dotenv.get("CALENDAR_API"))
                    .setTimeMin(now)
                    .setTimeMax(maxDate)
                    .setOrderBy("startTime")
                    .setSingleEvents(true)
                    .execute();
            List<Event> items = events.getItems();

            return items;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }

    }

    /**
     * Diese Methode verarbeitet die Events und schickt anschließend die Daten über den Publisher an den MQTT-Broker
     */
    public void sendMqttNachricht () {

        Database db = new Database();

        List<Raum> raeume = db.getRaeume();
        List<Event> events = getEvents();

        DateTime now = new DateTime(System.currentTimeMillis());
        SimpleDateFormat df = new SimpleDateFormat("HH:mm");

        List<Nachricht> buchungen = new ArrayList<>();

        /*
         * Für jedes Event wird eine Buchung erstellt. Außerdem wird das Start- und Endzeitpunkt benötigt. Damit wird
         * ermittelt ob es sich bei dem Event um ein Aktuelles Meeting handelt oder nicht.
         */
        for (Event event : events) {

            Nachricht buchung;

            //Hilfsklassen, das sich Datetime nicht formatieren lassen
            Date date1 = new Date(event.getStart().getDateTime().getValue());
            Date date2 = new Date(event.getEnd().getDateTime().getValue());

            String start = df.format(date1);
            String end = df.format(date2);
            String uhrzeit = start + "-" + end;

            //Wenn es einen Raum nicht in der Datenbank existiert, wird keine Buchung erstellt.
            if(db.getRaumByName(event.getLocation()) != null) {
                String summary = "";
                if(event.getSummary() != null) {
                    summary = event.getSummary();
                } else {
                    summary = "Meeting";
                }

                //Wenn ein Event zwischen Start und Endzeitpunkt exisiert wird Buchung mit Aktuelles Meeting True erstellt ansonsten mit False
                if (event.getStart().getDateTime().getValue() <= now.getValue() && event.getEnd().getDateTime().getValue() >= now.getValue()) {
                    buchung = new Nachricht(summary, uhrzeit, event.getCreator().getEmail(), true, db.getRaumByName(event.getLocation()));
                } else {
                    System.out.println(event.getSummary() != null);
                    buchung = new Nachricht(summary, uhrzeit, "", false, db.getRaumByName(event.getLocation()));
                }
                buchungen.add(buchung);
            }
        }

        /*
         * Für jeden existierenden Raum in der Datenbank wird nun eine sortierte Liste der einzelnen Events erstellt
         * Nachdem die sortierte Liste erstellt wird spezifisch der Anzahl der Events eine Funktion aufgerufen
         */
        for (Raum raum: raeume) {
            List<Nachricht> sortierteListe = new ArrayList<>();

            for(Nachricht buchung: buchungen) {

                if (raum.getRaumName().contains(buchung.getRaum().getRaumName())) {
                    sortierteListe.add(buchung);
                }
            }

            switch (sortierteListe.size()) {
                case 0:
                    erstelleNachrichtohneEvent(raum);
                    break;
                case 1:
                    erstelleNachrichteinEvent(raum, sortierteListe);
                    break;
                case 2:
                    erstelleNachrichtzweiEvent(raum,  sortierteListe);
                    break;
                case 3:
                    erstelleNachrichtdreiEvent(raum,  sortierteListe);
                    break;
                case 4:
                    erstelleNachrichtvierEvent(raum,  sortierteListe);
                    break;
                default:
                    erstelleNachrichtmehrEvent(raum, buchungen);
                    break;
            }
        }
        return;
    }

    /**
     * Wenn es für einen Raum kein Event gibt wird diese Funktion aufgerufen
     * @param raum Der Raum auf dem die Nachricht erstellt wird
     */
    private void erstelleNachrichtohneEvent(Raum raum) {

        //Erstellt 2 Nachrichten, Eine für ein aktuelles Freies Meeting und für keine existierenden Meetings
        Nachricht aktuellesMeeting = new Nachricht("Frei","", "");
        Nachricht meeting = new Nachricht("", (""));

        //Erstellt die MQTTNachricht
        MQTTNachricht mqttMessage = new MQTTNachricht(
                new JSONObject(aktuellesMeeting.getKeyValuePairMitVerantworlichen()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(getSystemInfo()));

        //Publisht die MQTT Nachricht
        Publisher pub = Publisher.getInstance();
        pub.sendNachricht(raum.getRaumTopic(), mqttMessage.buildMqttJson().toString(), 0);
    }

    /**
     * Wenn es für den Raum 1 Event gibt, wird diese Funktion aufgerufen
     * @param raum Der Raum auf dem die Nachricht erstellt wir
     * @param buchungen Sortierte Liste mit Buchungen für den angegeben Raum
     */
    private void erstelleNachrichteinEvent(Raum raum, List<Nachricht> buchungen) {

        JSONObject aktuellesMeeting;
        JSONObject meeting1;

        //Es wird überprüft, ob es ein Aktuelles Meeting gibt
        if(buchungen.get(0).isAktuellesMeeting()) {
            //Wenn ja wird ein JSON Object für das Aktuelle Meeting erstellt und die 1. Meeting Nachricht bleibt leer
            aktuellesMeeting = new JSONObject(buchungen.get(0).getKeyValuePairMitVerantworlichen());
            Nachricht meeting1Nachricht = new Nachricht("", (""));
            meeting1 = new JSONObject(meeting1Nachricht.getKeyValuePair());
        } else {
            //Falls nicht wird das Aktuelle Meeting als frei markiert und die das 1. Meeting enthält die Buchungsinformationen
            Nachricht aktuellesMeetingNachricht = new Nachricht("Frei", "", "");
            aktuellesMeeting = new JSONObject(aktuellesMeetingNachricht.getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(0).getKeyValuePair());
        }

        Nachricht meeting = new Nachricht("", (""));

        MQTTNachricht mqttMessage = new MQTTNachricht(
                aktuellesMeeting,
                meeting1,
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(getSystemInfo())
        );

        Publisher pub = Publisher.getInstance();
        pub.sendNachricht(raum.getRaumTopic(), mqttMessage.buildMqttJson().toString(), 0);

    }

    /**
     * Wenn es für den Raum 2 Event gibt, wird diese Funktion aufgerufen
     * @param raum Der Raum auf dem die Nachricht erstellt wir
     * @param buchungen Sortierte Liste mit Buchungen für den angegeben Raum
     */
    private void erstelleNachrichtzweiEvent(Raum raum, List<Nachricht> buchungen) {
        JSONObject aktuellesMeeting;
        JSONObject meeting1;
        JSONObject meeting2;

        //Es wird überprüft, ob es ein Aktuelles Meeting gibt
        if(buchungen.get(0).isAktuellesMeeting()) {
            aktuellesMeeting = new JSONObject(buchungen.get(0).getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(1).getKeyValuePair());

            Nachricht meeting2Nachricht = new Nachricht("", (""));
            meeting2 = new JSONObject(meeting2Nachricht.getKeyValuePair());
        } else {
            Nachricht aktuellesMeetingNachricht = new Nachricht("Frei", "", "");
            aktuellesMeeting = new JSONObject(aktuellesMeetingNachricht.getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(0).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(1).getKeyValuePair());
        }

        Nachricht meeting = new Nachricht("", (""));

        MQTTNachricht mqttMessage = new MQTTNachricht(
                aktuellesMeeting,
                meeting1,
                meeting2,
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(getSystemInfo())
        );

        Publisher pub = Publisher.getInstance();
        pub.sendNachricht(raum.getRaumTopic(), mqttMessage.buildMqttJson().toString(), 0);
    }

    /**
     * Wenn es für den Raum 3 Event gibt, wird diese Funktion aufgerufen
     * @param raum Der Raum auf dem die Nachricht erstellt wir
     * @param buchungen Sortierte Liste mit Buchungen für den angegeben Raum
     */
    private void erstelleNachrichtdreiEvent(Raum raum, List<Nachricht> buchungen) {
        JSONObject aktuellesMeeting;
        JSONObject meeting1;
        JSONObject meeting2;
        JSONObject meeting3;

        //Es wird überprüft, ob es ein Aktuelles Meeting gibt
        if(buchungen.get(0).isAktuellesMeeting()) {
            //Wenn Ja wird 1. Buchung zum Aktuelles Meeting und Buchung 1 und 2 zu Meeting 1-2. Meeting 3 und 4 bleiben frei
            aktuellesMeeting = new JSONObject(buchungen.get(0).getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(1).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(2).getKeyValuePair());

            Nachricht meeting3Nachricht = new Nachricht("", (""));
            meeting3 = new JSONObject(meeting3Nachricht.getKeyValuePair());
        } else {
            //Falls nicht ist aktuelles Meeting Frei und Buchung 1-3 gleich Meeting 1-3. Buchung 4 bleibt frei
            Nachricht aktuellesMeetingNachricht = new Nachricht("Frei", "", "");
            aktuellesMeeting = new JSONObject(aktuellesMeetingNachricht.getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(0).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(1).getKeyValuePair());
            meeting3 = new JSONObject(buchungen.get(2).getKeyValuePair());
        }

        Nachricht meeting = new Nachricht("", (""));

        MQTTNachricht mqttMessage = new MQTTNachricht(
                aktuellesMeeting,
                meeting1,
                meeting2,
                meeting3,
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(getSystemInfo())
        );

        Publisher pub = Publisher.getInstance();
        pub.sendNachricht(raum.getRaumTopic(), mqttMessage.buildMqttJson().toString(), 0);
    }

    /**
     * Wenn es für den Raum 4 Event gibt, wird diese Funktion aufgerufen
     * @param raum Der Raum auf dem die Nachricht erstellt wir
     * @param buchungen Sortierte Liste mit Buchungen für den angegeben Raum
     */
    private void erstelleNachrichtvierEvent(Raum raum, List<Nachricht> buchungen) {
        JSONObject aktuellesMeeting;
        JSONObject meeting1;
        JSONObject meeting2;
        JSONObject meeting3;
        JSONObject meeting4;

        //Es wird überprüft, ob es ein Aktuelles Meeting gibt
        if(buchungen.get(0).isAktuellesMeeting()) {
            //Wenn Ja wird die 1. Buchung zum aktuellen Meeting udn die 3 Folgenden Events zu Meeting 1-3. Das 4. Meeting bleibt frei
            aktuellesMeeting = new JSONObject(buchungen.get(0).getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(1).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(2).getKeyValuePair());
            meeting3 = new JSONObject(buchungen.get(3).getKeyValuePair());

            Nachricht meeting4Nachricht = new Nachricht("", (""));
            meeting4 = new JSONObject(meeting4Nachricht.getKeyValuePair());
        } else {
            //Falls nicht wird Buchung 1-4 gleich Meeting 1-4 und Aktuelles Meeting ist Frei
            Nachricht aktuellesMeetingNachricht = new Nachricht("Frei", "", "");
            aktuellesMeeting = new JSONObject(aktuellesMeetingNachricht.getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(0).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(1).getKeyValuePair());
            meeting3 = new JSONObject(buchungen.get(2).getKeyValuePair());
            meeting4 = new JSONObject(buchungen.get(3).getKeyValuePair());

        }

        MQTTNachricht mqttMessage = new MQTTNachricht(
                aktuellesMeeting,
                meeting1,
                meeting2,
                meeting3,
                meeting4,
                new JSONObject(getSystemInfo())
        );

        Publisher pub = Publisher.getInstance();
        pub.sendNachricht(raum.getRaumTopic(), mqttMessage.buildMqttJson().toString(), 0);
    }

    /**
     * Wenn es für den Raum mehr als 4 Events gibt, wird diese Funktion aufgerufen
     * @param raum Der Raum auf dem die Nachricht erstellt wir
     * @param buchungen Sortierte Liste mit Buchungen für den angegeben Raum
     */
    private void erstelleNachrichtmehrEvent(Raum raum, List<Nachricht> buchungen) {
        JSONObject aktuellesMeeting;
        JSONObject meeting1;
        JSONObject meeting2;
        JSONObject meeting3;
        JSONObject meeting4;

        //Es wird überprüft, ob es ein Aktuelles Meeting gibt
        if(buchungen.get(0).isAktuellesMeeting()) {
            //Falls Ja wird die 1. Buchung als Aktuelles Meeting erstellt und die nächsten 4 Folgenden Events sind Meeting 1-4
            aktuellesMeeting = new JSONObject(buchungen.get(0).getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(1).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(2).getKeyValuePair());
            meeting3 = new JSONObject(buchungen.get(3).getKeyValuePair());
            meeting4 = new JSONObject(buchungen.get(4).getKeyValuePair());
        } else {
            //Falls es kein Aktuelles Meeting gibt ist Buchung 1-4 gleich Meeting 1-4 und Aktuelles Meeting ist Frei
            Nachricht aktuellesMeetingNachricht = new Nachricht("Frei", "", "");
            aktuellesMeeting = new JSONObject(aktuellesMeetingNachricht.getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(0).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(1).getKeyValuePair());
            meeting3 = new JSONObject(buchungen.get(2).getKeyValuePair());
            meeting4 = new JSONObject(buchungen.get(3).getKeyValuePair());

        }

        MQTTNachricht mqttMessage = new MQTTNachricht(
                aktuellesMeeting,
                meeting1,
                meeting2,
                meeting3,
                meeting4,
                new JSONObject(getSystemInfo())
        );

        Publisher pub = Publisher.getInstance();
        pub.sendNachricht(raum.getRaumTopic(), mqttMessage.buildMqttJson().toString(), 0);
    }

    /**
     * Erstellt eine SystemInfo, zu welchem Datum und welcher Uhrzeit eine Nachricht erstellt wurde.
     * @return Map mit Aktuellem Datum und Uhrzeit
     */
    private Map getSystemInfo () {
        Date now = new Date(System.currentTimeMillis());
        SimpleDateFormat df1 = new SimpleDateFormat("dd.MM.yy");
        SimpleDateFormat df2 = new SimpleDateFormat("HH:mm");

        Map<String, String> systeminfo = new HashMap<>();
        systeminfo.put("datum", df1.format(now));
        systeminfo.put("updateUhrzeit", df2.format(now));

        return systeminfo;
    }


    /**
     * Thread Run methode die Überschrieben wurde von Runnable
     */
    @Override
    public void run() {
        while(true) {
            try {
                sendMqttNachricht();
                Thread.sleep(Integer.parseInt(dotenv.get("UPDATE_INTERVAL_IN_SEC"))*1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
