/***********************************************************************************************
 * Main Application source file
 * mostly only controll logic in this file, independent of the hardware or plattform this runs on
 * hardware dependency is abstracted by the single components ending in "..._esp32"
 **********************************************************************************************/


#include "tuerschild.h"



static const char* tag = "tuerschild application";

//unexpected failure always lands in STATE_ERR
typedef enum state
{
	STATE_INIT = 0,			//we begin here, do some basic initializations, go to STATE_GET_CONF
	STATE_GET_CONF,			//we try to find a valid configuration, if the user requests factory reset, factory settings are written and we start the config interface,
							//with the factory settings -> STATE_PREP_CONF
							//if the config interface without reset is reqeuested, we try to read the current configuration from flash and start the config interface,
							//with currently set AP_SSID, AP_PASS, and AP_CHANNEL -> STATE-PREP_CONF
							//if no configuration is requested, the current configuration is read and we try to get the MQTT message -> STATE_PREP_RECV
							//on failure to read configuration -> STATE_ERR
	 STATE_PREP_CONFIG,		//start the wifi access point with currently set AP_SSID, AP_PASS, and AP_CHANNEL,
	 						//start the httpd server providing the config interface ->STATE_CONF
							//if the accesspoint or the server fail -> STATE ERR
	STATE_CONFIG,			//wait untill a configuration has been received by the server, bring the server down, write the received configuration ton flash,
							//and try to get the mqtt message -> STATE_PREP_RECV
	STATE_PREP_RECV,		//prepare to receive the mqtt message, start the wifi client and join the configured netork, start the client and connect to the configured broker,
							//and subscribe the configured topic -> STATE_RECV
							//if any step fails -> STATE_ERR
	STATE_RECV,				//wait untill the mqtt message is returned -> STATE_DISPLAY
							//if an error or timeout occures -> STATE_ERR
	STATE_DISPLAY,			//parse and show the received message on the display, then go to sleep -> STATE_ENTERS_SLEEP,
	STATE_ENTER_SLEEP,		//go to sleep, on wake up we are again in -> STATE_INIT
	STATE_ERR				//reboot after error occured, after reboot, we start aain in -> START_INIT
}state_t;



void app_main(void)
{
	state_t state = STATE_INIT;
	
	tuerschild_config_t conf;

	tuerschild_conf_request_t conf_request;

	while(1) {
		switch(state) {
			
		case STATE_INIT:
			if(!early_init()) {		//some init(change watchdog time, nvs, event loop, paret of gpio)
				state = STATE_ERR;	//on failure reboot
			} else {
				init_empty_conf(&conf);	//create empty config
				conf_request = config_requested();	//check user request for configuration or factory_reset
				state = STATE_GET_CONF;				//next get configuration
				
			}
			
			break;
			
		case STATE_GET_CONF:
			if(conf_request == TUERSCHILD_CONF_REQUEST_RESET) {	//user requested factory reset
				TUERSCHILD_LOGI(tag, "doing full reset");
				factory_settings();				//write factory setting to flash
				read_conf_from_nvs(&conf);		//rad factory settings
				state = STATE_PREP_CONFIG;		//open config interface
			} else if(conf_request == TUERSCHILD_CONF_REQUEST_SIMPLE) {		//user wants config interface, with  old setttings
				TUERSCHILD_LOGI(tag, "bring config interface up");
				if(read_conf_from_nvs(&conf)) {		//read old settings
					state = STATE_PREP_CONFIG;		//open config interface
				} else {
					state = STATE_ERR;				//on failure reboot
				}
			} else if(read_conf_from_nvs(&conf)) {		//user wanted no config interface, we use old config to receive mqtt message
				TUERSCHILD_LOGI(tag, "proceed to reception");
				state = STATE_PREP_RECV;	//receive message
			} else {
				state = STATE_ERR;			//on failure reboot
			}
			conf_request = TUERSCHILD_CONF_REQUEST_NONE;		//clear request
			break;


		case STATE_PREP_RECV:
			if(!bring_wifi_up(TUERSCHILD_WIFI_STATION, &conf)) {		//start  wifi client
				state = STATE_ERR;	//failure
			} else if(!bring_mqtt_client_up(&conf))  {				//start mqtt client
				state = STATE_ERR;	//failure
			} else if(!start_mqtt_receive(&conf))  {		//subscribe
				state = STATE_ERR;	//failure
			} else {
				state = STATE_RECV;		//go on to  receive
			}
			break;
			
		case STATE_RECV:
			if(mqtt_error()) {		
				state = STATE_ERR;	//failure to receive
				TUERSCHILD_LOGE(tag, "mqtt error");
			} else if(mqtt_recv_success()) {
				state = STATE_DISPLAY;			//received
			} else if(mqtt_recv_timeout()) {
				state =  STATE_ERR;	//timeout
				TUERSCHILD_LOGE(tag, "mqtt timeout");
			} else {
				state = STATE_RECV; //wait for message
			}
			break;
		case STATE_DISPLAY:
			print_received();	
			process_and_show();	//show information on display
			
			state = STATE_ENTER_SLEEP;	//and go to sleep
			
			
			break;
			
		case STATE_ENTER_SLEEP:
			enter_sleep();	//sleep
			break;
		
		case STATE_PREP_CONFIG:
			TUERSCHILD_LOGI(tag, "opening config interface");
			if(!bring_wifi_up(TUERSCHILD_WIFI_AP, &conf)) {		//start hotspot
				state = STATE_ERR;	//failure
			} else if(!start_recv_config(&conf)) {			//start server
				state = STATE_ERR;	//failure
			} else {
				state = STATE_CONFIG; 
			}
			
			break;
			
		case STATE_CONFIG:

			if(!done_recv_conf()) {	
				state = STATE_CONFIG;		//wait for user unput
				tuerschild_delay_ms(1000);
			} else {
				stop_recv_config();			//stop server
				store_configuration(&conf); //save new config
				state = STATE_GET_CONF;
			}
			break;

		case STATE_ERR:
		TUERSCHILD_LOGE(tag, "OPERATION FEHLGESCHLAGEN, starte neu");
		reboot();
		break;

		default:	//should never happen
			state = STATE_ERR;
			tuerschild_delay_ms(1000);
			break;
		}
		TUERSCHILD_LOGI(tag, "state: %d", state);
		tuerschild_delay_ms(10);
		yield();
	}
}
