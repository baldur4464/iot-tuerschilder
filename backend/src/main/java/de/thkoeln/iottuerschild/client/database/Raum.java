package de.thkoeln.iottuerschild.client.database;

public class Raum {
    private int raumId;
    private String raumName;
    private String raumTopic;

    public Raum(int raumId, String raumName, String raumTopic) {
        this.raumId = raumId;
        this.raumName = raumName;
        this.raumTopic = raumTopic;
    }

    public int getRaumId() {
        return raumId;
    }

    public void setRaumId(int raumId) {
        this.raumId = raumId;
    }

    public String getRaumName() {
        return raumName;
    }

    public void setRaumName(String raumName) {
        this.raumName = raumName;
    }

    public String getRaumTopic() {
        return raumTopic;
    }

    public void setRaumTopic(String raumTopic) {
        this.raumTopic = raumTopic;
    }
}
