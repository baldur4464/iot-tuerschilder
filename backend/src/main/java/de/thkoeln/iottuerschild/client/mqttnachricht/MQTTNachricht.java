package de.thkoeln.iottuerschild.client.mqttnachricht;

import org.json.JSONObject;


/**
 * Diese Klasse fügt alle nötigen Information zusammen um eine Nachricht an den Broker zu versenden
 * @author Patrick Schmidt
 * @version 1.0
 * @since 15.06.2022
 */
public class MQTTNachricht {

    private JSONObject aktuellesMeeting;
    private JSONObject meeting1;
    private JSONObject meeting2;
    private JSONObject meeting3;
    private JSONObject meeting4;
    private JSONObject systeminfo;

    /**
     * Constructor für das MQTTNachricht-Objekt
     * @param aktuellesMeeting
     * @param meeting1
     * @param meeting2
     * @param meeting3
     * @param meeting4
     * @param systeminfo
     */
    public MQTTNachricht(JSONObject aktuellesMeeting, JSONObject meeting1, JSONObject meeting2, JSONObject meeting3, JSONObject meeting4, JSONObject systeminfo) {
        this.aktuellesMeeting = aktuellesMeeting;
        this.meeting1 = meeting1;
        this.meeting2 = meeting2;
        this.meeting3 = meeting3;
        this.meeting4 = meeting4;
        this.systeminfo = systeminfo;
    }

    public JSONObject getAktuellesMeeting() {
        return aktuellesMeeting;
    }

    public void setAktuellesMeeting(JSONObject aktuellesMeeting) {
        this.aktuellesMeeting = aktuellesMeeting;
    }

    public JSONObject getMeeting1() {
        return meeting1;
    }

    public void setMeeting1(JSONObject meeting1) {
        this.meeting1 = meeting1;
    }

    public JSONObject getMeeting2() {
        return meeting2;
    }

    public void setMeeting2(JSONObject meeting2) {
        this.meeting2 = meeting2;
    }

    public JSONObject getMeeting3() {
        return meeting3;
    }

    public void setMeeting3(JSONObject meeting3) {
        this.meeting3 = meeting3;
    }

    public JSONObject getMeeting4() {
        return meeting4;
    }

    public void setMeeting4(JSONObject meeting4) {
        this.meeting4 = meeting4;
    }

    public JSONObject getSystemInfo() {
        return systeminfo;
    }

    public void setSystemInfo(JSONObject systemInfo) {
        this.systeminfo = systemInfo;
    }

    /**
     * Erstellt aus den Informationen eine neues JSON-Object mit aktuellem Meeting, Meeting1, Meeting2, Meeting3, Meeting4
     * und systeminfo
     * @return JSONObject ein JSON-Object des MQTTNachricht Objekts
     */
    public JSONObject buildMqttJson () {
        JSONObject object = new JSONObject();
        object.put("aktuellesMeeting", this.aktuellesMeeting);
        object.put("meeting1", this.meeting1);
        object.put("meeting2", this.meeting2);
        object.put("meeting3", this.meeting3);
        object.put("meeting4", this.meeting4);
        object.put("systeminfo", this.systeminfo);

        return object;
    }

}
