package de.thkoeln.iottuerschild.client;

import de.thkoeln.iottuerschild.client.mqttnachricht.Nachricht;
import org.json.JSONArray;
import org.json.JSONObject;

public class JSONtest {

    public static void main (String... args) {

        Nachricht nachricht = new Nachricht("Test", "12:00 - 13:00");
        Nachricht nachricht2 = new Nachricht("Test 2", "14:00 - 16:00");

        JSONObject object = new JSONObject();
        JSONObject object2 = new JSONObject();
        JSONArray array = new JSONArray();

        object.put("meeting1", nachricht.getKeyValuePair());
        object.put("meeting2", nachricht2.getKeyValuePair());

        System.out.println(object.toString());

    }

}
