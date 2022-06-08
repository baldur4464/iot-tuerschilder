#include "tuerschild.h"

const char* tag = "tuerschild application";

typedef enum state
{
	STATE_INIT = 0, STATE_GET_CONF, STATE_PREP_CONFIG, STATE_CONFIG, STATE_PREP_RECV,
	STATE_RECV, STATE_PROCESS, STATE_DISPLAY, STATE_ENTER_SLEEP, STATE_ERR
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
			if(!bring_wifi_up(&conf)) {
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
			} else if(mqtt_recv_success()) {
				state = STATE_PROCESS;
			} else if(mqtt_recv_timeout()) {
				state =  STATE_ERR;
			}
			deallocate_configuration(&conf);
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
			state = STATE_CONFIG;
			break;
			
		case STATE_CONFIG:
			if(!dummy_conf()) {
				state = STATE_ERR;
			} else {
				state = STATE_GET_CONF;
			}
			break;
		default:
			state = STATE_ERR;
			break;
		}
		TUERSCHILD_LOGI(tag, "state: %d", state);
		yield();
	}
}
