#include "tuerschild.h"
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"


#define CONNECTED_BIT (1<<12)

static const char* tag = "tuerschild_wifi_esp32";



static EventGroupHandle_t wifi_events;

static esp_event_handler_instance_t handler_wifi;
static esp_event_handler_instance_t handler_ip;



static void event_handler_wifi(void* arg, esp_event_base_t base, int32_t event_id, void* event_data) {
	switch(event_id) {
		case WIFI_EVENT_STA_START:
			puts("station started");
			esp_wifi_connect();
			break;
		case WIFI_EVENT_STA_DISCONNECTED:
			puts("station disconnected, try reconnecting");
			esp_wifi_connect();
			break;
		case WIFI_EVENT_STA_CONNECTED:
			puts("connection successfull");
			break;
		default:
			printf("uncaught wifi event: %i\n", event_id);
		}
}

static void event_handler_ip(void* arg, esp_event_base_t base, int32_t event_id, void* event_data) {
	switch(event_id) {
		case IP_EVENT_STA_GOT_IP:
			puts("got ip");
			fflush(stdout);
			xEventGroupSetBits(wifi_events, CONNECTED_BIT);
			break;
		default: 
			printf("uncaught ip event: %i\n", event_id);
	}
}

int bring_wifi_up(tuerschild_config_t* conf)
{
	wifi_config_t wifi_cfg = {
		.sta = {
			//.ssid = conf->ssid,
			//.password = conf->password,
			.threshold.authmode = WIFI_AUTH_WPA2_PSK
		}
	};
	
	esp_netif_t* def_sta;
	esp_err_t error;
	EventBits_t event_bits;
	
	strcpy((char*)wifi_cfg.sta.ssid, conf->ssid);
	strcpy((char*)wifi_cfg.sta.password, conf->password);
	
		
	wifi_events = xEventGroupCreate();
	if(!wifi_events) {
		TUERSCHILD_LOGE(tag, "failed at creating event_group");
		return 0;
	}
	
	
	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();

	

	error = esp_event_handler_instance_register(
		WIFI_EVENT,	ESP_EVENT_ANY_ID, event_handler_wifi, NULL,
		&handler_wifi);
	//ESP_ERROR_CHECK(error);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at setting wifi handler");
		return 0;
	}
	
	error = esp_event_handler_instance_register(
		IP_EVENT,	ESP_EVENT_ANY_ID, event_handler_ip, NULL,
		&handler_ip);
	//ESP_ERROR_CHECK(error);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at setting ip handler");
		return 0;
	}
	
	error = esp_netif_init();
	//ESP_ERROR_CHECK(error);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at initializing network interface");
		return 0;
	}
	
	//~ does not return on failure
	//~ 'Creates default WIFI STA. In case of any init error this API aborts.'
	def_sta = esp_netif_create_default_wifi_sta();
	
	error = esp_wifi_init(&wifi_init_cfg);
	//ESP_ERROR_CHECK(error);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at initializing wifi");
		return 0;
	}
	
	error = esp_wifi_set_mode(WIFI_MODE_STA);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at setting wifi mode");
		return 0;
	}
	//ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	error = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at setting wifi config");
		return 0;
	}
	//ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
	
	error = esp_wifi_start();
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at starting wifi");
		return 0;
	}
	//ESP_ERROR_CHECK(esp_wifi_start());
	TUERSCHILD_LOGI(tag, "waiting for ip");
	
	event_bits = xEventGroupWaitBits(wifi_events, CONNECTED_BIT, pdTRUE, pdTRUE, 10000/portTICK_PERIOD_MS);
	if(!(event_bits & CONNECTED_BIT)) {
		TUERSCHILD_LOGE(tag, "getting ip address timed out");
		return 0;
	}
	TUERSCHILD_LOGI(tag, "got ip");
	return 1;
}
