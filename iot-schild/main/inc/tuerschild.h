#ifndef TUERSCHILD_H
#define TUERSCHILD_H

#include "tuerschild_esp32.h"

typedef enum tuerschild_conf_request
{
	TUERSCHILD_CONF_REQUEST_NONE,
	TUERSCHILD_CONF_REQUEST_SIMPLE,
	TUERSCHILD_CONF_REQUEST_RESET
} tuerschild_conf_request_t;

typedef enum tuerschild_wifi_mode
{
	TUERSCHILD_WIFI_STATION,
	TUERSCHILD_WIFI_AP,
	TUERSCHILD_WIFI_HYBRID
} tuerschild_wifi_mode_t;

#include <stdint.h>
typedef struct tuerschild_config
{
	//sta
	char* ssid;
	char* password;
	//mqtt
	char* broker;
	uint16_t port;
	char* topic;
	//ap
	char* ap_ssid;
	char* ap_password;
	uint8_t ap_channel;
	//ntp
	char* ntp_server;
} tuerschild_config_t;

#define TOPIC_IN_MAX_LEN 128
#define DATA_IN_MAX_LEN 4096

extern int topic_in_len;
extern int data_in_len;
extern char topic_in_buf[TOPIC_IN_MAX_LEN];
extern char data_in_buf[DATA_IN_MAX_LEN];

int early_init();
tuerschild_conf_request_t config_requested();

void init_empty_conf(tuerschild_config_t* conf);
void conf_set(tuerschild_config_t* conf, const char* ssid, const char* password, const char* broker, uint16_t port, const char* topic, const char* ap_ssid, const char* ap_pass, uint8_t ap_chan, const char* ntp);
void conf_set_ntp(tuerschild_config_t* conf, const char* ntp);
void conf_set_ssid(tuerschild_config_t* conf, const char* ssid);
void conf_set_pass(tuerschild_config_t* conf, const char* password);
void conf_set_broker(tuerschild_config_t* conf, const char* broker);
void conf_set_port(tuerschild_config_t* conf, const uint16_t port);
void conf_set_topic(tuerschild_config_t* conf, const char* topic);
void conf_set_ap_ssid(tuerschild_config_t* conf, const char* ap_ssid);
void conf_set_ap_pass(tuerschild_config_t* conf, const char* ap_pass);
void conf_set_ap_chan(tuerschild_config_t* conf, const uint8_t chan);
void set_empty_conf(tuerschild_config_t *conf);

int read_conf_from_nvs(tuerschild_config_t* conf);
int store_configuration(tuerschild_config_t* conf);
int factory_settings();
//int bring_wifi_station_up(tuerschild_config_t* conf);
//int bring_wifi_hybrid_up(tuerschild_config_t* conf);
int bring_wifi_up(tuerschild_wifi_mode_t mode, tuerschild_config_t* conf);
int hotspot_has_client();
int bring_mqtt_client_up(tuerschild_config_t* conf);
int start_mqtt_receive(tuerschild_config_t* conf);
int mqtt_recv_success();
int mqtt_recv_timeout();
int mqtt_error();
int start_recv_config(tuerschild_config_t* config);
int stop_recv_config();
int display();
void yield();
void enter_sleep();
int process();
int done_recv_conf();
void tuerschild_delay_ms(int x);

int init_reset_btn();

int reset_btn_pressed();


void setup_display();
void process_and_show();

void time_from_sntp();

void reboot();

#endif
