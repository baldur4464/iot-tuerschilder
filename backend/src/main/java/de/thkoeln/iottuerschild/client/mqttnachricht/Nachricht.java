package de.thkoeln.iottuerschild.client.mqttnachricht;


import java.util.HashMap;
import java.util.Map;

public class Nachricht{

    private String titel;
    private String uhrzeit;

    public Nachricht(String titel, String uhrzeit) {
        this.titel = titel;
        this.uhrzeit = uhrzeit;
    }

    public String getTitel() {
        return titel;
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
}
