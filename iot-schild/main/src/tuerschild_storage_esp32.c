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
	
	error = nvs_set_str(nvs_handle, "host", conf->broker);
	ret = ret && error == ESP_OK;
	
	error = nvs_set_u16(nvs_handle, "port", conf->port);
	ret = ret && error == ESP_OK;
	
	error = nvs_set_str(nvs_handle, "topic", conf->topic);
	ret = ret && error == ESP_OK;

	error = nvs_set_str(nvs_handle, "ap_ssid", conf->ap_ssid);
	ret = ret && error == ESP_OK;

	error = nvs_set_str(nvs_handle, "ap_pass", conf->ap_password);
	ret = ret && error == ESP_OK;

	error = nvs_set_u16(nvs_handle, "ap_chan", conf->ap_channel);
	ret = ret && error == ESP_OK;
	
	error = nvs_set_str(nvs_handle, "ntp", conf->ntp_server);
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

int read_conf_from_nvs(tuerschild_config_t* conf)
{
	esp_err_t error;
	nvs_handle_t nvs_handle;
	
	size_t length;
	
	char buf[512];
	uint16_t port;
	uint8_t chan;
	
	error = nvs_open("config", NVS_READWRITE, &nvs_handle);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to open nvs memory for reading, error %d", error);
		nvs_close(nvs_handle);
		return 0;
	}
	
	length = sizeof(buf);
	error = nvs_get_str(nvs_handle, "ssid", buf, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read ssid");
		nvs_close(nvs_handle);
		set_empty_conf(conf);
		return 0;
	}
	conf_set_ssid(conf, buf);

	length = sizeof(buf);
	error = nvs_get_str(nvs_handle, "pass", buf, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read pass");
		nvs_close(nvs_handle);
		set_empty_conf(conf);
		return 0;
	}
	conf_set_pass(conf, buf);
	
	length = sizeof(buf);
	error = nvs_get_str(nvs_handle, "host", buf, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read host");
		nvs_close(nvs_handle);
		set_empty_conf(conf);
		return 0;
	}
	conf_set_broker(conf, buf);


	error = nvs_get_u16(nvs_handle, "port", &port);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read");
		nvs_close(nvs_handle);
		set_empty_conf(conf);
		return 0;
	}
	conf_set_port(conf, port);

	length = sizeof(buf);
	error = nvs_get_str(nvs_handle, "topic", buf, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read");
		nvs_close(nvs_handle);
		set_empty_conf(conf);
		return 0;
	}
	conf_set_topic(conf, buf);

	length = sizeof(buf);
	error = nvs_get_str(nvs_handle, "ap_ssid", buf, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read");
		nvs_close(nvs_handle);
		set_empty_conf(conf);
		return 0;
	}
	conf_set_ap_ssid(conf, buf);

	length = sizeof(buf);
	error = nvs_get_str(nvs_handle, "ap_pass", buf, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read");
		nvs_close(nvs_handle);
		set_empty_conf(conf);
		return 0;
	}
	conf_set_ap_pass(conf, buf);

	
	error = nvs_get_u16(nvs_handle, "ap_chan", &chan);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read");
		nvs_close(nvs_handle);
		set_empty_conf(conf);
		return 0;
	}
	conf_set_ap_chan(conf, chan);

	length = sizeof(buf);
	error = nvs_get_str(nvs_handle, "ntp", buf, &length);
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed to read");
		nvs_close(nvs_handle);
		set_empty_conf(conf);
		return 0;
	}
	conf_set_ntp(conf, buf);


	nvs_close(nvs_handle);
	

	return 1;
}

