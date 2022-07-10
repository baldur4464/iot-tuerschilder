#ifndef TUERSCHILD_H
#define TUERSCHILD_H
/**********************************
 * definition of common types, declaration of common functions, and declaration of gloval buffers, see tuerschild_global and tuerschild_mqtt_*.c
 *	tuerschild_conf_request enum for the different config requests (none, only config, full factory reset) the user can make, see config_requested
 *	tuerschild_wifi_mode	type to communicate wifi mode to wifi component, see tuerschild_wifi_*.c
 *	tuerschild_config		structure holding the system configuration,	 see tuerschild_conf.c
 * 	early_init				call this before calling functions involving hardware
 * 	config_requested		returns the type of config-request the made with the button
 * 	conf_set*				set propertie of config structure see tuerschild_conf.c
 *	init_empty_conf			init empty config structure see tuerschild_conf.c
 *	set_empty_conf			delete all values  from config structure see tuerschild_conf.c
 * 	read_conf_from_nvs		see tuerschild_storage_*.c
 * 	store_configuration		see tuerschild_storage_*.c
 * 	factory_settings		writes default config to flash, see tuerschild_esp32.c
 * 	bring_wifi_up			see tuerschild_wifi_*.c
 * 	hotspot_has_client		see tuerschild_wifi_*.c
 * 	bring_mqtt_client_up	see tuerschild_mqtt_*.c
 *	start_mqtt_receive		see tuerschild_mqtt_*.c
 *	mqtt_recv_success		see tuerschild_mqtt_*.c
 *	mqtt_recv_timeout		see tuerschild_mqtt_*.c
 *	mqtt_error				see tuerschild_mqtt_*.c
 *	start_recv_config		see tuerschild_httpd_*.c
 *	stop_recv_config		see tuerschild_httpd_*.c
 *	done_recv_conf			see tuerschild_httpd_*.c
 *	print_received			prints received raw mqtt message 
 *	yield					returns controll to taskmanager, see tuerschild_esp32.c
 * 	enter_sleep				enter deep sleep for 15 minutes
 *	tuerschild_delay_ms		pause task
 *	init_reset_btn			init input pin for conig button
 *	reset_btn_pressed		true if config button is pressed
 *	process_and_show		parses received mqtt message and shows it on display, see tuerschild_eink_*.c and *_eink_driver*
 *	time_from_sntp			syncs system clock 
*	reboot					reboot
 *********************************/
#include "tuerschild_esp32.h"
#include <stdint.h>


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
extern char topic_in_buf[TOPIC_IN_MAX_LEN+1];
extern char data_in_buf[DATA_IN_MAX_LEN+1];

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
int print_received();
void yield();
void enter_sleep();
int done_recv_conf();
void tuerschild_delay_ms(int x);

int init_reset_btn();

int reset_btn_pressed();


void setup_display();
void process_and_show();

void time_from_sntp();

void reboot();

#endif
