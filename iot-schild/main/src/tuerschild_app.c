/**************************************************************
 * Testbench for the Tuerschild-System
 * 
 * a complete run tests all software components  of the system
 * if capable a test will detect failure and stop halt execution with an error message indicating the line of the failure.
 * Following tests are implemented:
 * 	test_config
 * 		tests initialization and setting of the configuration structure
 * 		runs and fails autonomous
 * 	test_storage
 * 		tests writng and reading configuration to nvs, can't work if  test_config fails
 * 		runs and fails autonomous
 * 	test_wifi 
 * 		tests the wifi hotspot and station capabilities, can't run without configuration
 * 		needs the tester to connect to the hotspot
 * 		needs the tester to enter connection info for testing the station
 * 	test_config_request
 * 		tests if requesting the configuration interface works
 * 		needs the tester to push the config-request button for different periods
 * 	test_subscriber
 * 		tests if the system can connect to a mqtt-broker and receive messages, needs wifi-station to be up
 *		needs the tester to enter connection hostname, port, and topic
 *		needs the tester to compare the messages
 *	test_config_interface
 *		tests if the configuration interface over html forms works
 *		needs the tester to connect to the hotspot and navigate to the configuration site
 *		needs the tester to enter test values
 *		the tester has to compare the received values
 *	process_and_show
 *		tests if the the GUI is displayed correctly
 *		needs the tester to check, if the correct design is displayed
 *		needs the tester to check correctness of values
 * 
 *************************************************************/

#include "tuerschild.h"

#define TEST_EXPECT(exp) \
{\
	if(!(exp)) {\
		TUERSCHILD_LOGE(tag, "Test Failed an line: %d,  staying still...", __LINE__);\
		while(1);\
	}\
}

#include <string.h>
#include <stdio.h>


void my_ngets(char* put, int size);

void eat_stdin();

void test_config();
void test_storage();
void test_wifi();

void test_config_request();

void test_subscriber();

void test_config_interface();

static const char* tag = "tuerschild testbench";



	const char json[] = "{\"meeting1\":{ \"titel\":\"Schreiben der Dokumentation \", \"uhrzeit\":\"16:00-17:00\"}, \"meeting2\":{ \"titel\":\"Ueberpruefung, ob alle Anforderungen erfuellt wurden \", \"uhrzeit\":\"17:00-18:00\"}, \"systeminfo\":{ \"datum\":\"09.07.2022 \", \"updateUhrzeit\":\"15:42\"}, \"meeting3\":{ \"titel\":\"Finale Fassung analysieren \", \"uhrzeit\":\"20:00-21:00\"}, \"meeting4\":{ \"titel\":\"Endbesprechung der Tuerschilddokumentation \", \"uhrzeit\":\"22:00-23:00\"}, \"aktuellesMeeting\":{ \"titel\":\"aktuellen Stand besprechen \", \"verantwortlicher\":\"ESP Gruppe IOTuerschild\", \"uhrzeit\":\"15:00-16:00\"}}";
	const char topic[] = "Raum 5";

void app_main(void)
{

	eat_stdin();
	
	TUERSCHILD_LOGI(tag, "****Internet of Tuerschild testbench****");
	TEST_EXPECT(early_init());
	
	test_config_request();
	test_config();
	test_storage();
	
	test_wifi();
	test_config_interface();
	test_subscriber();


	
	strcpy(topic_in_buf, topic);
	strcpy(data_in_buf, json);
	process_and_show();
	
	TUERSCHILD_LOGI(tag, "tests done");
	while(1) {
		yield();
	}
}


void test_config_request()
{
	TUERSCHILD_LOGI(tag, "+Testing config request+");

	puts("///please hold the config-request-button,  press enter, and release the button in less than 5 seconds after///");
	while(getchar() != 0xA) {
		yield();
	}
	TEST_EXPECT(config_requested() == TUERSCHILD_CONF_REQUEST_NONE);

	puts("///please hold the config-request-button,  press enter, and release the button in 5 to 10  seconds after///");
	while(getchar() != 0xA) {
		yield();
	}
	TEST_EXPECT(config_requested() == TUERSCHILD_CONF_REQUEST_SIMPLE);

	puts("///please hold the config-request-button,  press enter, and release the button in 10 or more seconds///");
	while(getchar() != 0xA) {
		yield();
	}
	TEST_EXPECT(config_requested() == TUERSCHILD_CONF_REQUEST_RESET);

	
}


void test_config()
{
	const uint8_t ap_channel = 6;
	const char* ap_password = "ap password 235";
	const char* ap_ssid = "ap ssid 99";
	const char* broker = "broker hostname is a stirng too";
	const char* ntp_server = "never actually used in the system at this point";
	const char* password = "this is the password to join wifi, when normal mode";
	uint16_t port = 999;
	const char* ssid = " i will join this wifi";
	const char* topic = "i will subscribe";

	tuerschild_config_t conf;

	TUERSCHILD_LOGI(tag, "+Testing configuration+");
	TUERSCHILD_LOGI(tag, "init empty");
	init_empty_conf(&conf);
	TEST_EXPECT(!conf.ap_password);
	TEST_EXPECT(!conf.ap_ssid);
	TEST_EXPECT(!conf.broker);
	TEST_EXPECT(!conf.ntp_server);
	TEST_EXPECT(!conf.password);
	TEST_EXPECT(!conf.ssid);
	TEST_EXPECT(!conf.topic);

	TUERSCHILD_LOGI(tag, "setting");
	conf_set(&conf, ssid, password, broker, port, topic, ap_ssid, ap_password, ap_channel, ntp_server);

	TUERSCHILD_LOGI(tag, "reading");
	TEST_EXPECT(conf.ap_channel == ap_channel);
	TEST_EXPECT(!strcmp(conf.ap_password, ap_password));
	TEST_EXPECT(!strcmp(conf.ap_ssid, ap_ssid));
	TEST_EXPECT(!strcmp(conf.broker, broker));
	TEST_EXPECT(!strcmp(conf.ntp_server, ntp_server));
	TEST_EXPECT(!strcmp(conf.password, password));
	TEST_EXPECT(conf.port == port);
	TEST_EXPECT(!strcmp(conf.ssid, ssid));
	TEST_EXPECT(!strcmp(conf.topic, topic));

	TUERSCHILD_LOGI(tag, "freeing");
	set_empty_conf(&conf);
	TEST_EXPECT(!conf.ap_password);
	TEST_EXPECT(!conf.ap_ssid);
	TEST_EXPECT(!conf.broker);
	TEST_EXPECT(!conf.ntp_server);
	TEST_EXPECT(!conf.password);
	TEST_EXPECT(!conf.ssid);
	TEST_EXPECT(!conf.topic);

}

void test_storage()
{
	const tuerschild_config_t conf_written = {
		.ap_channel = 6,
		.ap_password = "ap password 235",
		.ap_ssid = "ap ssid 99",
		.broker = "broker hostname is a stirng too",
		.ntp_server = "never actually used in the system at this point",
		.password = "this is the password to join wifi, when normal mode",
		.port = 999,
		.ssid = " i will join this wifi",
		.topic = "i will subscribe"
	};
	tuerschild_config_t conf_read;
	TUERSCHILD_LOGI(tag, "+Testing storage+");
	TUERSCHILD_LOGI(tag, "writing...");
	store_configuration(&conf_written);
	
	TUERSCHILD_LOGI(tag, "reading...");
	init_empty_conf(&conf_read);
	read_conf_from_nvs(&conf_read);

	TEST_EXPECT(conf_read.ap_channel == conf_written.ap_channel);
	TEST_EXPECT(!strcmp(conf_read.ap_password, conf_written.ap_password));
	TEST_EXPECT(!strcmp(conf_read.ap_ssid, conf_written.ap_ssid));
	TEST_EXPECT(!strcmp(conf_read.broker, conf_written.broker));
	TEST_EXPECT(!strcmp(conf_read.ntp_server, conf_written.ntp_server));
	TEST_EXPECT(!strcmp(conf_read.password, conf_written.password));
	TEST_EXPECT(conf_read.port == conf_written.port);
	TEST_EXPECT(!strcmp(conf_read.ssid, conf_written.ssid));
	TEST_EXPECT(!strcmp(conf_read.topic, conf_written.topic));

	set_empty_conf(&conf_read);
}

void test_wifi()
{
	
	tuerschild_config_t conf  = {
		.ssid = "testTuerschild",
		.password = "11234567",
		.broker = "broker_hostname",
		.port = 1883,
		
		.topic = "raum/5",

		
		.ap_ssid = "testTuerschild",
		.ap_password = "11234567",
		.ap_channel = 6,
		.ntp_server = "pool.ntp.org"
	};


	TUERSCHILD_LOGI(tag, "+Testing Hotspot+");
	
	//TEST_EXPECT(store_configuration(&factory_conf));

	//factory_settings();

	//tuerschild_config_t conf;	
	//init_empty_conf(&conf);


	//TEST_EXPECT(read_conf_from_nvs(&conf));

	TUERSCHILD_LOGI(tag, "+start hotspot+");
	TEST_EXPECT(bring_wifi_up(TUERSCHILD_WIFI_AP, &conf));
	//printf("%s, %s\n", factory_conf.ap_password, factory_conf.ap_ssid);

	printf("///please connect to hotspot \"%s\", with password \"%s\", wifichannel should be %d///\n", conf.ap_ssid , conf.ap_password, conf.ap_channel );
	

	while(!hotspot_has_client()) {
		yield();
	}
	TUERSCHILD_LOGI(tag, "connection established");

	char ssid[200];
	char pass[200];
	
	
	
	TUERSCHILD_LOGI(tag, "+Testing Station+");

	TUERSCHILD_LOGI(tag, "connecting to my self");

	puts("///enter SSID, to which to connect: ///\n");
	my_ngets(ssid, 199);
	puts("///enter passwordfor connection: ///\n");
	my_ngets(pass, 199);

	//conf_set_ssid(&conf, ssid);
	//conf_set_pass(&conf, pass);
	conf.ssid = ssid;
	conf.password = pass;

	TUERSCHILD_LOGI(tag, "try connecting to, %s with %s", conf.ssid, conf.password);
	TEST_EXPECT(bring_wifi_up(TUERSCHILD_WIFI_STATION, &conf));

	//set_empty_conf(&conf);
}



void test_subscriber()
{
	char broker[200];
	char topic[200];
	char port_str[20];

	tuerschild_config_t conf;

	TUERSCHILD_LOGI(tag, "+Testing MQTT-Subscriber+");

	puts("///enter hostname for broker which holds a message for us:///\n");
	my_ngets(broker, 199);
	conf.broker = broker;

	puts("///enter port of broker which holds a message for us:///\n");
	my_ngets(port_str, 199);
	conf.port = atoi(port_str);

	puts("///enter topic to subscribe:///\n");
	my_ngets(topic, 199);
	conf.topic = topic;
	TUERSCHILD_LOGI(tag, "receiving");
	TEST_EXPECT(bring_mqtt_client_up(&conf));
	TEST_EXPECT(start_mqtt_receive(&conf));

	while(!mqtt_recv_success()) {
		TEST_EXPECT(!mqtt_error());
		TEST_EXPECT(!mqtt_recv_timeout());
	};
	TUERSCHILD_LOGI(tag, "recieved message:\n%s\n on topic:\n%s", data_in_buf, topic_in_buf);
}

void test_config_interface()
{
	tuerschild_config_t conf;
	init_empty_conf(&conf);

	TUERSCHILD_LOGI(tag, "+Testing Configuration interface+");
	TUERSCHILD_LOGI(tag, "Starting server");
	
	TEST_EXPECT(start_recv_config(&conf));
	puts("///please open the site '192.168.4.1/conf and enter test values///");

	while(!done_recv_conf()) {
		tuerschild_delay_ms(10);
	}

	printf("///the values you entered were:///\n"
	"access_point channel: %d\n access_point password: %s\naccess_point ssid: %s\nbroker: %s\nntp_server: %s\n"
	"password: %s\nport: %d\nssid: %s\ntopic: %s\n", conf.ap_channel, conf.ap_password, conf.ap_ssid, conf.broker, conf.ntp_server, conf.password, conf.port, conf.ssid, conf.topic);

	set_empty_conf(&conf);
}


void my_ngets(char* put, int size)

{
	int a = EOF;
	while(size > 1 && a != 0xa) {
		a = getchar();
		if(a != 0xa && a!= EOF) {
			*(put++) = a;
			size--;
			putchar(a);
			fflush(stdout);
		}
		tuerschild_delay_ms(10);
	}
	*put = 0;
	putchar(a);
}

void eat_stdin()
{
	while(getchar() != EOF);
}
