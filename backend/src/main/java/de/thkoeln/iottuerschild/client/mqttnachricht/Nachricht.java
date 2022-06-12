package de.thkoeln.iottuerschild.client.mqttnachricht;


import de.thkoeln.iottuerschild.client.database.Raum;

import java.util.HashMap;
import java.util.Map;

public class Nachricht{

    private String titel;
    private String verantwortlicher;
    private String uhrzeit;
    private Raum raum;
    private boolean aktuellesMeeting;

    public Nachricht(String titel, String uhrzeit, String verantwortlicher, boolean aktuellesMeeting, Raum raum) {
        this.titel = titel;
        this.uhrzeit = uhrzeit;
        this.verantwortlicher = verantwortlicher;
        this.aktuellesMeeting = aktuellesMeeting;
        this.raum = raum;
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

    public Map getKeyValuePair () {
        Map<String, String>map = new HashMap<>();
        map.put("uhrzeit", this.uhrzeit);
        map.put("titel", this.titel);

        return map;
    }

    public Map getKeyValuePairMitVerantworlichen () {
        Map<String, String> map = new HashMap<>();
        map.put("titel", this.titel);
        map.put("uhrzeit", this.uhrzeit);
        map.put("verantwortlicher", this.verantwortlicher);

        return map;
    }
}
