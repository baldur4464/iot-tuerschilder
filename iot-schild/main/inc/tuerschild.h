#ifndef TUERSCHILD_H
#define TUERSCHILD_H

#include "tuerschild_esp32.h"

typedef enum tuerschild_wifi_mode
{
	TUERSCHILD_WIFI_STATION,
	TUERSCHILD_WIFI_AP,
	TUERSCHILD_WIFI_HYBRID
} tuerschild_wifi_mode_t;

#include <stdint.h>
typedef struct tuerschild_config
{
	char* ssid;
	char* password;
	
	char* hostname;
	uint16_t port;
	
	char* topic;
} tuerschild_config_t;

#define TOPIC_IN_MAX_LEN 128
#define DATA_IN_MAX_LEN 1024

extern volatile int topic_in_len;
extern volatile int data_in_len;
extern volatile char topic_in_buf[TOPIC_IN_MAX_LEN];
extern volatile char data_in_buf[DATA_IN_MAX_LEN];

int early_init();
int config_requested();
int allocate_and_load_configuration(tuerschild_config_t* conf);
void deallocate_configuration(tuerschild_config_t* conf);
int store_configuration(tuerschild_config_t* conf);
int dummy_conf();
//int bring_wifi_station_up(tuerschild_config_t* conf);
//int bring_wifi_hybrid_up(tuerschild_config_t* conf);
int bring_wifi_up(tuerschild_wifi_mode_t mode, tuerschild_config_t* conf);
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
#endif
