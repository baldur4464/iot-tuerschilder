#include "tuerschild.h"

#include <memory.h>

#include "esp_check.h"
#include "nvs_flash.h"

static const char* tag = "tuerschild_storage_esp32";

#define SSID_OVERLEN 33
#define PASS_OVERLEN 65
#define HOSTNAME_OVERLEN 254
#define TOPIC_OVERLEN (TOPIC_IN_MAX_LEN+1)

/*********************************************
 * data format:
 * 	ssid : n*uint8_t
 * 	pass : n*uint8_t
 * 	host : n*uint8_t
 * 	port : uint16_t
 * 	topic : n*uint8_t
 *********************************************/


int store_configuration(tuerschild_config_t* conf)
{
	esp_err_t error;
	nvs_handle_t nvs_handle;
	
	int ret = 1;

	error = nvs_open("config", NVS_READWRITE, &nvs_handle);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to open nvs memory for writing");
		nvs_close(nvs_handle);
		return 0;
	}
	
	error = nvs_set_str(nvs_handle, "ssid", conf->ssid);
	ret = ret && error == ESP_OK;
	
	error = nvs_set_str(nvs_handle, "pass", conf->password);
	ret = ret && error == ESP_OK;
	
	error = nvs_set_str(nvs_handle, "host", conf->hostname);
	ret = ret && error == ESP_OK;
	
	error = nvs_set_u16(nvs_handle, "port", conf->port);
	ret = ret && error == ESP_OK;
	
	error = nvs_set_str(nvs_handle, "topic", conf->topic);
	ret = ret && error == ESP_OK;
	
	if(!ret) {
		TUERSCHILD_LOGE(tag, "failed at writing");
		nvs_close(nvs_handle);
		return 0;
	}
	error = nvs_commit(nvs_handle);
	nvs_close(nvs_handle);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "commit to memory failed");
		return 0;
	}
	return 1;
};

int allocate_and_load_configuration(tuerschild_config_t* conf)
{
	esp_err_t error;
	nvs_handle_t nvs_handle;
	
	int ret = 1;
	size_t length;
	
	conf->ssid = NULL;
	conf->password = NULL;
	conf->hostname = NULL;
	conf->topic = NULL;
	
	error = nvs_open("config", NVS_READWRITE, &nvs_handle);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to open nvs memory for reading");
		nvs_close(nvs_handle);
		return 0;
	}
	
	error = nvs_get_str(nvs_handle, "ssid", NULL, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read ssid len");
		nvs_close(nvs_handle);
		return 0;
	}
	conf->ssid = malloc(length);
	error = nvs_get_str(nvs_handle, "ssid", conf->ssid, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read ssid");
		nvs_close(nvs_handle);
		deallocate_configuration(conf);
		return 0;
	}
	
	error = nvs_get_str(nvs_handle, "pass", NULL, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read pass len");
		nvs_close(nvs_handle);
		deallocate_configuration(conf);
		return 0;
	}
	conf->password = malloc(length);
	error = nvs_get_str(nvs_handle, "pass", conf->password, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read pass");
		nvs_close(nvs_handle);
		deallocate_configuration(conf);
		return 0;
	}
	
	error = nvs_get_str(nvs_handle, "host", NULL, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read host len");
		nvs_close(nvs_handle);
		deallocate_configuration(conf);
		return 0;
	}
	conf->hostname = malloc(length);
	error = nvs_get_str(nvs_handle, "host", conf->hostname, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read host");
		nvs_close(nvs_handle);
		deallocate_configuration(conf);
		return 0;
	}
	

	
	error = nvs_get_u16(nvs_handle, "port", &conf->port);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read");
		nvs_close(nvs_handle);
		deallocate_configuration(conf);
		return 0;
	}
	
	error = nvs_get_str(nvs_handle, "topic", NULL, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read");
		nvs_close(nvs_handle);
		deallocate_configuration(conf);
		return 0;
	}
	conf->topic = malloc(length);
	error = nvs_get_str(nvs_handle, "topic", conf->topic, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read");
		nvs_close(nvs_handle);
		deallocate_configuration(conf);
		return 0;
	}
	
	nvs_close(nvs_handle);
	

	return 1;
}

void deallocate_configuration(tuerschild_config_t *conf)
{
	if(conf->ssid) {
		free(conf->ssid);
	}
	if(conf->password) {
		free(conf->password);
	}
	if(conf->hostname) {
		free(conf->hostname);
	}
	if(conf->topic) {
		free(conf->topic);
	}
}
