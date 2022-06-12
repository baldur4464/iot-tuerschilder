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
import de.thkoeln.iottuerschild.client.mqttnachricht.Nachricht;


import java.io.*;
import java.lang.reflect.Array;
import java.security.GeneralSecurityException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;

public class CelendarAPI implements Runnable{
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
        InputStream in = CelendarAPI.class.getResourceAsStream(CREDENTIALS_FILE_PATH);
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

        CelendarAPI api = new CelendarAPI();

        api.sendMqttNachricht();

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
                    .set("location", "Raum 5")
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
        SimpleDateFormat df = new SimpleDateFormat("kk:mm");

        List<Nachricht> buchungen = new ArrayList<>();

        for (Event event: events) {

            Nachricht buchung;

            //Hilfsklassen, das sich Datetime nicht formatieren lassen
            Date date1 = new Date(event.getStart().getDateTime().getValue());
            Date date2 = new Date(event.getEnd().getDateTime().getValue());

            String start = df.format(date1);
            String end = df.format(date2);
            String uhrzeit = start+" - " + end;

            if (event.getStart().getDateTime().getValue() <= now.getValue() && event.getEnd().getDateTime().getValue() >= now.getValue()) {
                System.out.println(event.getLocation());
                buchung = new Nachricht(event.getSummary(), uhrzeit, event.getCreator().getEmail(), true, db.getRaumByName(event.getLocation()));
            } else {
                System.out.println(event.getLocation());
                buchung = new Nachricht(event.getSummary(), uhrzeit, "", false, db.getRaumByName(event.getLocation()));
            }
            buchungen.add(buchung);
        }



        for(Nachricht buchung: buchungen) { ;
            System.out.println(buchung.getKeyValuePairMitVerantworlichen() + " Raum: " + buchung.getRaum().getRaumName());
        }


        for(Raum raum: raeume) {

        }


    }

    @Override
    public void run() {

    }
}
