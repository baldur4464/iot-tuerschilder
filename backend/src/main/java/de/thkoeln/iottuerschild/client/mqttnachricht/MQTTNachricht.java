package de.thkoeln.iottuerschild.client.mqttnachricht;

import org.json.JSONObject;



public class MQTTNachricht {

    private String topic;

    private JSONObject aktuellesMeeting;
    private JSONObject meeting1;
    private JSONObject meeting2;
    private JSONObject meeting3;
    private JSONObject meeting4;
    private JSONObject systemInfo;

    public MQTTNachricht(JSONObject aktuellesMeeting, JSONObject meeting1, JSONObject meeting2, JSONObject meeting3, JSONObject meeting4, JSONObject systemInfo) {
        this.aktuellesMeeting = aktuellesMeeting;
        this.meeting1 = meeting1;
        this.meeting2 = meeting2;
        this.meeting3 = meeting3;
        this.meeting4 = meeting4;
        this.systemInfo = systemInfo;
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
        return systemInfo;
    }

    public void setSystemInfo(JSONObject systemInfo) {
        this.systemInfo = systemInfo;
    }


}
