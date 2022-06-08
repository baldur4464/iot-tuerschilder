package de.thkoeln.iottuerschild.client.mqttnachricht;

import org.json.JSONObject;

import java.util.Date;

public class MQTTNachricht {
    private String titel;
    private String topic;
    private String verantwortlicher;
    private String raum;
    private String uhrzeitAnfang;
    private String uhrzeitEnde;



    public String getTitel() {
        return titel;
    }

    public void setTitel(String titel) {
        this.titel = titel;
    }

    public String getTopic() {
        return topic;
    }

    public void setTopic(String topic) {
        this.topic = topic;
    }

    public String getUhrzeitAnfang() {
        return uhrzeitAnfang;
    }

    public void setUhrzeitAnfang(String uhrzeitAnfang) {
        this.uhrzeitAnfang = uhrzeitAnfang;
    }

    public String getUhrzeitEnde() {
        return uhrzeitEnde;
    }

    public void setUhrzeitEnde(String uhrzeitEnde) {
        this.uhrzeitEnde = uhrzeitEnde;
    }

    public String getRaum() {
        return raum;
    }

    public void setRaum(String raum) {
        this.raum = raum;
    }

    public MQTTNachricht(String titel, String topic, String uhrzeitAnfang, String uhrzeitEnde, String raum) {
        this.titel = titel;
        this.topic = topic;
        this.uhrzeitAnfang = uhrzeitAnfang;
        this.uhrzeitEnde = uhrzeitEnde;
        this.raum = raum;
    }

    public JSONObject nachrichtToJSON () {

        JSONObject jsonObject = new JSONObject();

        jsonObject.put("titel", this.titel);
        jsonObject.put("uhrzeit", this.uhrzeitAnfang + "-" + this.uhrzeitEnde);
        jsonObject.put("raum", this.raum);

        return jsonObject;
    }

}
