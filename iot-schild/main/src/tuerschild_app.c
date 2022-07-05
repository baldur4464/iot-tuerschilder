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
				init_empty_conf(&conf);
				state = STATE_GET_CONF;
			}
			break;
			
		case STATE_GET_CONF:
			if(!read_conf_from_nvs(&conf) ) {
				TUERSCHILD_LOGI(tag, "conf not read");
				dummy_conf();
				if(read_conf_from_nvs(&conf)){
					TUERSCHILD_LOGE(tag, "can't read config I just wrote");
					state = STATE_PREP_CONFIG;	
				}else{
					state = STATE_ERR;
				}
				
			} else if( config_requested()) {
				TUERSCHILD_LOGI(tag, "conf requested");
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
				//state = STATE_RESERVED;
			}
			
			break;
			
		case STATE_ENTER_SLEEP:
			enter_sleep();
			break;
		
		case STATE_PREP_CONFIG:
			TUERSCHILD_LOGI(tag, "opening config interface");
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
			time_from_sntp();
			state = STATE_ENTER_SLEEP;
		break;
		default:
			state = STATE_ERR;
			tuerschild_delay_ms(1000);
			break;
		}
		TUERSCHILD_LOGI(tag, "state: %d", state);
		tuerschild_delay_ms(1000);
		//time_from_sntp();
		yield();
	}
}
