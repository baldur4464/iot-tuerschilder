#include "tuerschild.h"

#include <string.h>



static const char* tag = "tuerschild application_testing";




void app_main(void)
{
	early_init();

	const char* json = "{\"meeting1\":{ \"titel\":\"Schreiben der Dokumentation \", \"uhrzeit\":\"16:00-17:00\"}, \"meeting2\":{ \"titel\":\"Ueberpruefung, ob alle Anforderungen erfuellt wurden \", \"uhrzeit\":\"17:00-18:00\"}, \"systeminfo\":{ \"datum\":\"09.07.2022 \", \"updateUhrzeit\":\"15:42\"}, \"meeting3\":{ \"titel\":\"Finale Fassung analysieren \", \"uhrzeit\":\"20:00-21:00\"}, \"meeting4\":{ \"titel\":\"Endbesprechung der Tuerschilddokumentation \", \"uhrzeit\":\"22:00-23:00\"}, \"aktuellesMeeting\":{ \"titel\":\"aktuellen Stand besprechen \", \"verantwortlicher\":\"ESP Gruppe IOTuerschild\", \"uhrzeit\":\"15:00-16:00\"}}";
	const char* topic = "Raum 5";
	
	strcpy(topic_in_buf, topic);
	strcpy(data_in_buf, json);
	process_and_show();
	
	TUERSCHILD_LOGI(tag, "tests done");
	while(1) {
		yield();
	}
}
