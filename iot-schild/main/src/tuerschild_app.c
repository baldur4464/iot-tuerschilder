#include "tuerschild.h"

const char* tag = "tuerschild application";

#warning dummy state
typedef enum state
{
	STATE_INIT = 0, STATE_GET_CONF, STATE_PREP_CONFIG, STATE_CONFIG, STATE_PREP_RECV,
	STATE_RECV, STATE_PROCESS, STATE_DISPLAY, STATE_ENTER_SLEEP, STATE_ERR, STATE_RESERVED
}state_t;



void app_main(void)
{
	state_t state = STATE_INIT;
	
	tuerschild_config_t conf;
	
	while(1) {
		switch(state) {
			
		case STATE_INIT:
			if(!early_init()) {
				state = STATE_ERR;
			} else {
				state = STATE_GET_CONF;
			}
			break;
			
		case STATE_GET_CONF:
			if(config_requested() || !allocate_and_load_configuration(&conf)) {
				state = STATE_PREP_CONFIG;
			} else {
				state = STATE_PREP_RECV;
			}
			break;
			
		case STATE_PREP_RECV:
			if(!bring_wifi_up(TUERSCHILD_WIFI_STATION, &conf)) {
				state = STATE_ERR;
			} else if(!bring_mqtt_client_up(&conf))  {
				state = STATE_ERR;
			} else if(!start_mqtt_receive(&conf))  {
				state = STATE_ERR;
			} else {
				state = STATE_RECV;
			}
			break;
			
		case STATE_RECV:
			if(mqtt_error()) {
				state = STATE_ERR;
				TUERSCHILD_LOGE(tag, "mqtt error");
			} else if(mqtt_recv_success()) {
				state = STATE_PROCESS;
			} else if(mqtt_recv_timeout()) {
				state =  STATE_ERR;
				TUERSCHILD_LOGE(tag, "mqtt timeout");
			}
			break;
		case STATE_PROCESS:
			if(!process()) {
				state = STATE_ERR;
			} else {
				state = STATE_DISPLAY;
			}
			break;
		case STATE_DISPLAY:
			if(!display()) {
				state = STATE_ERR;
			} else {
				state = STATE_ENTER_SLEEP;
			}
			
			break;
			
		case STATE_ENTER_SLEEP:
			enter_sleep();
			break;
		
		case STATE_PREP_CONFIG:
			if(!bring_wifi_up(TUERSCHILD_WIFI_AP, &conf)) {
				state = STATE_ERR;
			} else if(!start_recv_config(&conf)) {
				state = STATE_ERR;
			} else {
				state = STATE_CONFIG;
			}
			
			/*if(!allocate_and_load_configuration(&conf)) {
				state = STATE_ERR;
			} else if(!bring_wifi_up(TUERSCHILD_WIFI_HYBRID, &conf)) {
				state = STATE_ERR;
			} else {
				state = STATE_GET_CONF;
			}*/
			/*if(!bring_wifi_hybrid_up()) {
				state = STATE_ERR;

			} else if(!start_recv_config()) {
				state = STATE_ERR;
			} else {
				state = STATE_CONFIG;
			}*/
			//state = STATE_CONFIG;
			TUERSCHILD_LOGW(tag, "waiting with only ap");
			tuerschild_delay_ms(10000);
			break;
			
		case STATE_CONFIG:
			/*if(!dummy_conf()) {
				state = STATE_ERR;
			} else {
				state = STATE_GET_CONF;
			}*/
			if(!done_recv_conf()) {
				state = STATE_CONFIG;
				tuerschild_delay_ms(1000);
			} else {
				stop_recv_config();
				store_configuration(&conf);
				state = STATE_GET_CONF;
			}
			break;
		case STATE_RESERVED:
			
		break;
		default:
			state = STATE_ERR;
			tuerschild_delay_ms(1000);
			break;
		}
		TUERSCHILD_LOGI(tag, "state: %d", state);
		yield();
	}
}
