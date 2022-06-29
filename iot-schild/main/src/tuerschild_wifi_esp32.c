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



static EventGroupHandle_t wifi_events = NULL;

static esp_event_handler_instance_t handler_wifi = NULL;
static esp_event_handler_instance_t handler_ip = NULL; 

static esp_netif_t* def_sta;
static esp_netif_t* def_ap;

static int  init_done  = 0;


static int _common_wifi_init(wifi_init_config_t* wifi_init_cfg, EventGroupHandle_t* wifi_events, esp_event_handler_t event_handler_wifi, esp_event_handler_instance_t* handler_wifi
	, esp_event_handler_t event_handler_ip, esp_event_handler_instance_t* handler_ip)
{
	esp_err_t error;

	error = esp_wifi_stop();

	if(!*wifi_events) {
		*wifi_events = xEventGroupCreate();
	}
	if(!*wifi_events) {
		TUERSCHILD_LOGE(tag, "failed at creating event_group");
		return 0;
	}
	
	const wifi_init_config_t def_conf = WIFI_INIT_CONFIG_DEFAULT();
	(*wifi_init_cfg) = def_conf;

	
	if(!*handler_wifi) {
		error = esp_event_handler_instance_register(
			WIFI_EVENT,	ESP_EVENT_ANY_ID, event_handler_wifi, NULL,
			handler_wifi);
		
		//ESP_ERROR_CHECK(error);
		if(error != ESP_OK) {
			TUERSCHILD_LOGE(tag, "failed at setting wifi handler");
			return 0;
		}
	}
	if(!*handler_ip) {
		error = esp_event_handler_instance_register(
			IP_EVENT,	ESP_EVENT_ANY_ID, event_handler_ip, NULL,
			handler_ip);
		//ESP_ERROR_CHECK(error);
		if(error != ESP_OK) {
			TUERSCHILD_LOGE(tag, "failed at setting ip handler");
			return 0;
		}
	}
	error = esp_netif_init();
	//ESP_ERROR_CHECK(error);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at initializing network interface");
		return 0;
	}
	return 1;
}

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

/*int bring_wifi_station_up(tuerschild_config_t* conf)
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
	
	wifi_init_config_t wifi_init_cfg;
	
	strcpy((char*)wifi_cfg.sta.ssid, conf->ssid);
	strcpy((char*)wifi_cfg.sta.password, conf->password);
	
		
	if(!_common_wifi_init(&wifi_init_cfg, &wifi_events , event_handler_wifi, &handler_wifi, event_handler_ip, &handler_ip)) {
		return 0;
	}
	
	//~ does not return on failure
	//~ 'Creates default WIFI STA. In case of any init error this call aborts.'
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
}*/

int bring_wifi_up(tuerschild_wifi_mode_t mode, tuerschild_config_t* conf)
{
	TUERSCHILD_LOGI(tag, "enter wifi_hybrid");
	wifi_config_t wifi_cfg_sta = {
		.sta = {
			//.ssid = conf->ssid,
			//.password = conf->password,
			.threshold.authmode = WIFI_AUTH_WPA2_PSK
		}
	};

	wifi_config_t wifi_cfg_ap = {
        .ap = {
            //.ssid = conf->ap_ssid,
            .ssid_len = strlen(conf->ap_ssid),
            .channel = conf->ap_channel,
            //.password = conf->ap_password,
            .max_connection = 1,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
	};

	strcpy((char*)wifi_cfg_ap.ap.ssid, conf->ap_ssid);
	strcpy((char*)wifi_cfg_ap.ap.password, conf->ap_password);
	

	esp_err_t error = ESP_OK;
	EventBits_t event_bits;
	wifi_init_config_t wifi_init_cfg;

	wifi_mode_t old_mode, new_mode;



	
	if(mode == TUERSCHILD_WIFI_STATION || mode == TUERSCHILD_WIFI_HYBRID) {
		strcpy((char*)wifi_cfg_sta.sta.ssid, conf->ssid);
		strcpy((char*)wifi_cfg_sta.sta.password, conf->password);
	}
		
	if(!_common_wifi_init(&wifi_init_cfg, &wifi_events , event_handler_wifi, &handler_wifi, event_handler_ip, &handler_ip)) {
		return 0;
	}

	TUERSCHILD_LOGI(tag, "done common wifi init");
	//~ does not return on failure
	//~ 'Creates default WIFI STA. In case of any init error this call aborts.'
	if((!def_sta) && (mode == TUERSCHILD_WIFI_STATION || mode ==TUERSCHILD_WIFI_HYBRID)) {
		def_sta = esp_netif_create_default_wifi_sta();
	}
	if((!def_ap) && (mode == TUERSCHILD_WIFI_AP || mode ==TUERSCHILD_WIFI_HYBRID)) {
		def_ap = esp_netif_create_default_wifi_ap();
	}

	TUERSCHILD_LOGI(tag, "created interfaces");

	if(!init_done) {
		error = esp_wifi_init(&wifi_init_cfg);
		//ESP_ERROR_CHECK(error);
		if(error != ESP_OK) {
			TUERSCHILD_LOGE(tag, "failed at initializing wifi");
			return 0;
		}
		esp_wifi_set_mode(WIFI_MODE_NULL);
		TUERSCHILD_LOGI(tag, "wifi init done ");
		init_done = 1;
	}
	
	error = esp_wifi_get_mode(&old_mode);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at getting old wifi mode");
		return 0;
	}

	if(old_mode == WIFI_MODE_APSTA || mode == TUERSCHILD_WIFI_HYBRID || (mode == TUERSCHILD_WIFI_AP && old_mode == WIFI_MODE_STA) || (mode == TUERSCHILD_WIFI_STATION && old_mode == WIFI_MODE_AP)) {
		new_mode = WIFI_MODE_APSTA;
	} else if(mode == TUERSCHILD_WIFI_AP) {
		new_mode = WIFI_MODE_AP;
	} else {
		new_mode = WIFI_MODE_STA;
	};
	TUERSCHILD_LOGI(tag, "new wifi mode is: %d", new_mode);
	esp_wifi_stop();
	switch(mode)
	{
		case TUERSCHILD_WIFI_AP:
			//error = esp_wifi_set_mode(WIFI_MODE_AP);
			TUERSCHILD_LOGI(tag, "wifi AP");
			break;
		case TUERSCHILD_WIFI_STATION:
			//error = esp_wifi_set_mode(WIFI_MODE_STA);
			TUERSCHILD_LOGI(tag, "wifi station");
			break;
		case TUERSCHILD_WIFI_HYBRID:
			//error = esp_wifi_set_mode(WIFI_MODE_APSTA);
			TUERSCHILD_LOGI(tag, "wifi hybrid");
			break;
	}
	esp_wifi_set_mode(new_mode);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at setting wifi mode error :%d", error);
		return 0;
	}
	

	if(mode == TUERSCHILD_WIFI_STATION || mode == TUERSCHILD_WIFI_HYBRID) {
		error = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg_sta);
		if(error != ESP_OK) {
			TUERSCHILD_LOGE(tag, "failed at setting wifi station config");
			return 0;
		}
	}
	if(mode == TUERSCHILD_WIFI_AP || mode == TUERSCHILD_WIFI_HYBRID) {
		error = esp_wifi_set_config(WIFI_IF_AP, &wifi_cfg_ap);
		if(error != ESP_OK) {
			TUERSCHILD_LOGE(tag, "failed at setting wifi ap config");
			return 0;
		}
	}

	error = esp_wifi_start();
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at starting wifi");
		return 0;
	}

	if(mode == TUERSCHILD_WIFI_HYBRID || mode == TUERSCHILD_WIFI_STATION) {
		event_bits = xEventGroupWaitBits(wifi_events, CONNECTED_BIT, pdTRUE, pdTRUE, 10000/portTICK_PERIOD_MS);
		if(!(event_bits & CONNECTED_BIT)) {
			TUERSCHILD_LOGE(tag, "getting ip address timed out");
			return 0;
		}
		TUERSCHILD_LOGI(tag, "got ip");
	}
	esp_netif_ip_info_t ip_info;
	error =  esp_netif_get_ip_info(def_ap, &ip_info);
	if(error == ESP_OK) {
		TUERSCHILD_LOGW(tag, "AP ip is: " IPSTR, IP2STR(&ip_info.ip));
	} else {
		TUERSCHILD_LOGE(tag, "failed to get ip info for AP");
	}

	return 1;
}