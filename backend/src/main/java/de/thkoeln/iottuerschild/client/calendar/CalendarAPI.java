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
import org.checkerframework.checker.units.qual.A;
import org.json.JSONObject;


import java.io.*;
import java.security.GeneralSecurityException;
import java.text.SimpleDateFormat;
import java.util.*;

public class CalendarAPI implements Runnable{
    /** Application name. */
    private static final String APPLICATION_NAME = "Google Calendar API Java Quickstart";
    /** Global instance of the JSON factory. */
    private static final JsonFactory JSON_FACTORY = GsonFactory.getDefaultInstance();
    /** Directory to store authorization tokens for this application. */
    private static final String TOKENS_DIRECTORY_PATH = "tokens";

    /**
     * Global instance of the scopes required by this quickstart.
     * If modifying these scopes, delete your previously saved tokens/ folder.
     */
    private static final List<String> SCOPES = Collections.singletonList(CalendarScopes.CALENDAR_READONLY);
    private static final String CREDENTIALS_FILE_PATH = "/credentials.json";


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

    public static void main(String... args) throws IOException, GeneralSecurityException {
        // Build a new authorized API client service.

        CalendarAPI api = new CalendarAPI();
        api.sendMqttNachricht();
        System.exit(1);
    }

    public List getEvents () {
        try {
            final NetHttpTransport HTTP_TRANSPORT = GoogleNetHttpTransport.newTrustedTransport();
            Calendar service = new Calendar.Builder(HTTP_TRANSPORT, JSON_FACTORY, getCredentials(HTTP_TRANSPORT))
                    .setApplicationName(APPLICATION_NAME)
                    .build();

            // List the next 10 events from the primary calendar.
            Date nowDate = new Date(System.currentTimeMillis());

            java.util.Calendar cal = java.util.Calendar.getInstance();
            cal.setTime(nowDate);
            cal.add(java.util.Calendar.HOUR_OF_DAY, 19);
            Date maxNow = cal.getTime();

            DateTime now = new DateTime(nowDate);
            DateTime maxDate = new DateTime(maxNow);

            Events events = service.events().list("9r74t1cf6i83ibqpa443q7flh0@group.calendar.google.com")
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

    public void sendMqttNachricht () {

        Database db = new Database();

        List<Raum> raeume = db.getRaeume();
        List<Event> events = getEvents();

        DateTime now = new DateTime(System.currentTimeMillis());
        SimpleDateFormat df = new SimpleDateFormat("HH:mm");

        List<Nachricht> buchungen = new ArrayList<>();

        for (Event event : events) {

            Nachricht buchung;

            //Hilfsklassen, das sich Datetime nicht formatieren lassen
            Date date1 = new Date(event.getStart().getDateTime().getValue());
            Date date2 = new Date(event.getEnd().getDateTime().getValue());

            String start = df.format(date1);
            String end = df.format(date2);
            String uhrzeit = start + "-" + end;

            if(db.getRaumByName(event.getLocation()) != null) {
                if (event.getStart().getDateTime().getValue() <= now.getValue() && event.getEnd().getDateTime().getValue() >= now.getValue()) {
                    buchung = new Nachricht(event.getSummary(), uhrzeit, event.getCreator().getEmail(), true, db.getRaumByName(event.getLocation()));
                } else {
                    buchung = new Nachricht(event.getSummary(), uhrzeit, "", false, db.getRaumByName(event.getLocation()));
                }
                buchungen.add(buchung);
            }
        }

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

    private void erstelleNachrichtohneEvent(Raum raum) {

        Nachricht aktuellesMeeting = new Nachricht("Frei","", "");
        Nachricht meeting = new Nachricht("", (""));

        MQTTNachricht mqttMessage = new MQTTNachricht(
                new JSONObject(aktuellesMeeting.getKeyValuePairMitVerantworlichen()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(meeting.getKeyValuePair()),
                new JSONObject(getSystemInfo()));

        Publisher pub = Publisher.getInstance();
        pub.sendNachricht(raum.getRaumTopic(), mqttMessage.buildMqttJson().toString(), 0);
    }

    private void erstelleNachrichteinEvent(Raum raum, List<Nachricht> buchungen) {

        JSONObject aktuellesMeeting;
        JSONObject meeting1;

        if(buchungen.get(0).isAktuellesMeeting()) {
            aktuellesMeeting = new JSONObject(buchungen.get(0).getKeyValuePairMitVerantworlichen());
            Nachricht meeting1Nachricht = new Nachricht("", (""));
            meeting1 = new JSONObject(meeting1Nachricht.getKeyValuePair());
        } else {
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

    private void erstelleNachrichtzweiEvent(Raum raum, List<Nachricht> buchungen) {
        JSONObject aktuellesMeeting;
        JSONObject meeting1;
        JSONObject meeting2;

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

    private void erstelleNachrichtdreiEvent(Raum raum, List<Nachricht> buchungen) {
        JSONObject aktuellesMeeting;
        JSONObject meeting1;
        JSONObject meeting2;
        JSONObject meeting3;

        if(buchungen.get(0).isAktuellesMeeting()) {
            aktuellesMeeting = new JSONObject(buchungen.get(0).getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(1).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(2).getKeyValuePair());

            Nachricht meeting3Nachricht = new Nachricht("", (""));
            meeting3 = new JSONObject(meeting3Nachricht.getKeyValuePair());
        } else {
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

    private void erstelleNachrichtvierEvent(Raum raum, List<Nachricht> buchungen) {
        JSONObject aktuellesMeeting;
        JSONObject meeting1;
        JSONObject meeting2;
        JSONObject meeting3;
        JSONObject meeting4;

        if(buchungen.get(0).isAktuellesMeeting()) {
            aktuellesMeeting = new JSONObject(buchungen.get(0).getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(1).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(2).getKeyValuePair());
            meeting3 = new JSONObject(buchungen.get(3).getKeyValuePair());

            Nachricht meeting4Nachricht = new Nachricht("", (""));
            meeting4 = new JSONObject(meeting4Nachricht.getKeyValuePair());
        } else {
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

    private void erstelleNachrichtmehrEvent(Raum raum, List<Nachricht> buchungen) {
        JSONObject aktuellesMeeting;
        JSONObject meeting1;
        JSONObject meeting2;
        JSONObject meeting3;
        JSONObject meeting4;

        if(buchungen.get(0).isAktuellesMeeting()) {
            aktuellesMeeting = new JSONObject(buchungen.get(0).getKeyValuePairMitVerantworlichen());
            meeting1 = new JSONObject(buchungen.get(1).getKeyValuePair());
            meeting2 = new JSONObject(buchungen.get(2).getKeyValuePair());
            meeting3 = new JSONObject(buchungen.get(3).getKeyValuePair());
            meeting4 = new JSONObject(buchungen.get(4).getKeyValuePair());
        } else {
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

    private Map getSystemInfo () {
        Date now = new Date(System.currentTimeMillis());
        SimpleDateFormat df1 = new SimpleDateFormat("dd.MM.yy");
        SimpleDateFormat df2 = new SimpleDateFormat("HH:mm");

        Map<String, String> systeminfo = new HashMap<>();
        systeminfo.put("datum", df1.format(now));
        systeminfo.put("updateUhrzeit", df2.format(now));

        return systeminfo;
    }


    @Override
    public void run() {
        while(true) {
            try {
                sendMqttNachricht();
                Thread.sleep(10000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
