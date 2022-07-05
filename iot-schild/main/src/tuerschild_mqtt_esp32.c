#include "tuerschild.h"

#include "time.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "mqtt_client.h"


static const char* tag = "tuerschild_mqtt_esp32";

#define MQTT_UP_BIT (1<<13)
#define MQTT_SUBSCRIBED_BIT (1<<14)
#define MQTT_RECV_SUCCESS (1)
#define MQTT_ERROR_BIT (2)

static EventGroupHandle_t mqtt_events;

static esp_mqtt_client_handle_t client;

static time_t start_recv;





static void event_handler_mqtt(void* arg, esp_event_base_t base, int32_t event_id, void* event_data) {
	esp_mqtt_event_handle_t event = event_data;
	switch(event_id) {
		#warning TODO:segmented mqtt messages
		case MQTT_EVENT_DATA:
			
			if(event->topic_len > TOPIC_IN_MAX_LEN || event->total_data_len > DATA_IN_MAX_LEN) {
				TUERSCHILD_LOGE(tag, "unsupported data length");
				xEventGroupSetBits(mqtt_events, MQTT_ERROR_BIT);
			} else if(event->data_len != event->total_data_len) {
				TUERSCHILD_LOGE(tag, "segmented data not yet supported");
				xEventGroupSetBits(mqtt_events, MQTT_ERROR_BIT);
			} else {
				#warning no mutex, datarace
				topic_in_len = event->topic_len;
				data_in_len = event->data_len;
				memcpy(topic_in_buf, event->topic, topic_in_len);
				memcpy(data_in_buf, event->data, data_in_len);
				data_in_buf[data_in_len] = 0;
				xEventGroupSetBits(mqtt_events, MQTT_RECV_SUCCESS);
			}
			break;
		case MQTT_EVENT_SUBSCRIBED:
			xEventGroupSetBits(mqtt_events, MQTT_SUBSCRIBED_BIT);
			break;
		case MQTT_EVENT_CONNECTED:
			xEventGroupSetBits(mqtt_events, MQTT_UP_BIT);
			break;
		case MQTT_EVENT_ERROR:
			xEventGroupSetBits(mqtt_events, MQTT_ERROR_BIT);
			break;
		default: 
			TUERSCHILD_LOGW(tag, "uncaught mqtt event: %i\n", event_id);
	}
}

int bring_mqtt_client_up(tuerschild_config_t* conf)
{
	esp_mqtt_client_config_t mqtt_cfg = {
		.host = conf->broker,
		.transport = MQTT_TRANSPORT_OVER_TCP,
		.port = conf->port
	};
	esp_err_t error;
	EventBits_t event_bits;
	mqtt_events = xEventGroupCreate();
	if(!mqtt_events) {
		TUERSCHILD_LOGE(tag, "failed at creating event_group");
		return 0;
	}
	
	client = esp_mqtt_client_init(&mqtt_cfg);
	if(!client) {
		TUERSCHILD_LOGE(tag, "failed at creating client");
		return 0;
	}
	
	error = esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, event_handler_mqtt, NULL);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at registering event handler");
		return 0;
	}
	
	error = esp_mqtt_client_start(client);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at starting client");
		return 0;
	}
	event_bits = xEventGroupWaitBits(mqtt_events, MQTT_UP_BIT, pdTRUE, pdTRUE, 10000/portTICK_PERIOD_MS);
	if(!(event_bits & MQTT_UP_BIT)) {
		TUERSCHILD_LOGE(tag, "connect timeout");
		return 0;
	}
	return 1;
}
int start_mqtt_receive(tuerschild_config_t* conf)
{
	EventBits_t event_bits;
	esp_mqtt_client_subscribe(client, conf->topic, 1);
	event_bits = xEventGroupWaitBits(mqtt_events, MQTT_SUBSCRIBED_BIT, pdTRUE, pdTRUE, 10000/portTICK_PERIOD_MS);
	if(!(event_bits & MQTT_SUBSCRIBED_BIT)) {
		TUERSCHILD_LOGE(tag, "subscribe timeout");
		return 0;
	}
	time(&start_recv);
	return 1;
}
int mqtt_recv_success()
{
	EventBits_t event_bits;
	event_bits = xEventGroupGetBits(mqtt_events);
	return !!(event_bits & MQTT_RECV_SUCCESS);
}
int mqtt_recv_timeout() {
	time_t now;
	time(&now);
	return now - start_recv > 10;
}

int mqtt_error()
{
	EventBits_t event_bits;
	event_bits =  xEventGroupGetBits(mqtt_events);
	return !!(event_bits & MQTT_ERROR_BIT);
}
