package de.thkoeln.iottuerschild.client.mqttnachricht;


import de.thkoeln.iottuerschild.client.database.Raum;

import java.util.HashMap;
import java.util.Map;

/**
 * Abbildung einer Nachricht für das MQTTNachricht Objekt
 * @author Patrick Schmidt
 * @version 1.0
 * @since 15.06.2022
 */
public class Nachricht{

    private String titel; //Meeting Titelname
    private String verantwortlicher; //Email oder Name des Verantwortlichen
    private String uhrzeit; //Uhrzeit des Meetings
    private Raum raum; //Raum in welches das Meeting stattfindet
    private boolean aktuellesMeeting; //Ist das Meeting ein aktuelles Meeting oder nicht

    /**
     * Constructor der Nachricht
     * @param titel
     * @param uhrzeit
     * @param verantwortlicher
     * @param aktuellesMeeting
     * @param raum
     */
    public Nachricht(String titel, String uhrzeit, String verantwortlicher, boolean aktuellesMeeting, Raum raum) {
        this.titel = titel;
        this.uhrzeit = uhrzeit;
        this.verantwortlicher = verantwortlicher;
        this.aktuellesMeeting = aktuellesMeeting;
        this.raum = raum;
    }

    /**
     * Constructor der Nachricht, wenn klar ist, dass es kein Aktuelles Meeting ist
     * @param titel
     * @param uhrzeit
     */
    public Nachricht(String titel, String uhrzeit) {
        this.titel = titel;
        this.uhrzeit = uhrzeit;
        this.verantwortlicher = "";
        this.aktuellesMeeting = false;
        this.raum = null;
    }

    /**
     * Constructor der Nachricht, wenn klar ist, dass es ein Aktuelles Meeting ist
     * @param titel
     * @param uhrzeit
     * @param verantwortlicher
     */
    public Nachricht (String titel, String uhrzeit, String verantwortlicher) {
        this.titel = titel;
        this.uhrzeit = uhrzeit;
        this.verantwortlicher = verantwortlicher;
    }

    public String getTitel() {
        return titel;
    }

    public String getVerantwortlicher() {
        return verantwortlicher;
    }

    public void setVerantwortlicher(String verantwortlicher) {
        this.verantwortlicher = verantwortlicher;
    }

    public Raum getRaum() {
        return raum;
    }

    public void setRaum(Raum raum) {
        this.raum = raum;
    }

    public boolean isAktuellesMeeting() {
        return aktuellesMeeting;
    }

    public void setAktuellesMeeting(boolean aktuellesMeeting) {
        this.aktuellesMeeting = aktuellesMeeting;
    }

    public void setTitel(String titel) {
        this.titel = titel;
    }

    public String getUhrzeit() {
        return uhrzeit;
    }

    public void setUhrzeit(String uhrzeit) {
        this.uhrzeit = uhrzeit;
    }

    /**
     * Gibt ein KeyValuePair zurück um daraus ein JSONObject zu bauen
     * @return Map
     */
    public Map getKeyValuePair () {
        Map<String, String>map = new HashMap<>();
        map.put("uhrzeit", this.uhrzeit);
        map.put("titel", this.titel);

        return map;
    }

    /**
     * Gibt ein KeyValuePair zurück um draus ein JSONObject zu bauen für ein aktuelles Meeting
     * @return
     */
    public Map getKeyValuePairMitVerantworlichen () {
        Map<String, String> map = new HashMap<>();
        map.put("titel", this.titel);
        map.put("uhrzeit", this.uhrzeit);
        map.put("verantwortlicher", this.verantwortlicher);

        return map;
    }
}
