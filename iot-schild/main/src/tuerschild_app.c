#include "tuerschild.h"

#include <string.h>



static const char* tag = "tuerschild application_testing";




void app_main(void)
{
	early_init();

	const char* json = "{\"meeting1\":{ \"titel\":\"Meeting 1 \", \"uhrzeit\":\"14:00-15:00\"}, \"meeting2\":{ \"titel\":\"Meeting 2 \", \"uhrzeit\":\"15:00-16:00\"}, \"systeminfo\":{ \"datum\":\"04.07.2022 \", \"updateUhrzeit\":\"15:42\"}, \"meeting3\":{ \"titel\":\"Meeting 3 \", \"uhrzeit\":\"17:00-18:00\"}, \"meeting4\":{ \"titel\":\"Meeting 4 \", \"uhrzeit\":\"18:00-19:00\"}, \"aktuellesMeeting\":{ \"titel\":\"Abschlussvortrag \", \"verantwortlicher\":\"Stefan Slooten\", \"uhrzeit\":\"13:00-14:00\"}}";
	const char* topic = "Hot Topic";
	
	strcpy(topic_in_buf, topic);
	strcpy(data_in_buf, json);
	process_and_show();
}
