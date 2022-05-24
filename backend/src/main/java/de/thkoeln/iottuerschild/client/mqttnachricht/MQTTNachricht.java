package de.thkoeln.iottuerschild.client.mqttnachricht;

import java.util.Date;

public class MQTTNachricht {
    private String titel;
    private String topic;
    private String uhrzeitAnfang;
    private String uhrzeitEnde;
    private String ort;

    public MQTTNachricht() {

    }

    public MQTTNachricht(String titel, String topic, String uhrzeitAnfang, String uhrzeitEnde, String ort) {
        this.titel = titel;
        this.topic = topic;
        this.uhrzeitAnfang = uhrzeitAnfang;
        this.uhrzeitEnde = uhrzeitEnde;
        this.ort = ort;
    }


}
