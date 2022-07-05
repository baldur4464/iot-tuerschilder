#include "tuerschild.h"

#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_check.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "nvs_flash.h"

#include "sntp.h"

#include "Arduino.h"

static const char* tag= "tuerschild_esp32";

int early_init()
{
	int ret = 1;
	esp_err_t error;

	//initArduino();
	
	
	error = nvs_flash_init();
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at bringing flash memory up");
		ret = 0;
	}
	
	error = esp_event_loop_create_default();
	if(error != ESP_OK) {
		TUERSCHILD_LOGE(tag, "failed at creating event loop");
		ret = 0;
	}

	
	return ret;
}
RTC_NOINIT_ATTR static int request = 1;
int config_requested()
{
	//TODO implement
	
	TUERSCHILD_LOGW(tag, "using dummy function 'config_requested'");
	if(request) {
		request = 0;
		return 1;
	}
	return 0;
}

int valid_config()
{
	//TODO implement
	TUERSCHILD_LOGW(tag, "using dummy function 'valid_config'");
	return 1;
}

void yield()
{
	#warning dummy yield
	//TODO implement
	//TUERSCHILD_LOGW(tag, "dummy yield function");
	vTaskDelay(100/portTICK_PERIOD_MS);
}


void tuerschild_delay_ms(int x)
{
	#warning dummy delay;
	//TODO implement
	//TUERSCHILD_LOGW(tag, "dummy delay function");
	vTaskDelay(x/portTICK_PERIOD_MS);
}

#warning no mutex, datarace
int display()
{
	//TODO implement
	TUERSCHILD_LOGW(tag, "dummy display function");
	TUERSCHILD_LOGI(tag, "%.*s", topic_in_len, topic_in_buf);
	TUERSCHILD_LOGI(tag, "%.*s", data_in_len, data_in_buf);
	return 1;
}

void enter_sleep()
{
	#warning enter_sleep to implement
	//TODO implement
	
	//TUERSCHILD_LOGW(tag, "waiting");
	//vTaskDelay(20000/portTICK_PERIOD_MS);
	//struct timeval time;
	//gettimeofday(&time, NULL);
	//TUERSCHILD_LOGW(tag, "enter sleep at epoch: %ld.%06ld", time.tv_sec, time.tv_usec);
	esp_deep_sleep(10*1000000);
}

int process()
{
	//TODO implement
	TUERSCHILD_LOGW(tag, "dummy processing function");
	return 1;
}

int dummy_conf()
{
	//TODO implement

#warning hardcoded config

#include "dummy_conf.inc"
	

	TUERSCHILD_LOGW(tag, "writing dummy config");
	
	return store_configuration(&dummy_conf);
}

void time_from_sntp()
{
	sntp_sync_status_t status;
	struct timeval time;

	gettimeofday(&time, NULL);
	TUERSCHILD_LOGW(tag, "epoch befor sync: %ld.%06ld", time.tv_sec, time.tv_usec);
	
	sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
	while((status = sntp_get_sync_status()) != SNTP_SYNC_STATUS_COMPLETED) {
		tuerschild_delay_ms(1000);
		TUERSCHILD_LOGW(tag, "waiting for ntp, syn status: %d", status);
	}
	gettimeofday(&time, NULL);
	TUERSCHILD_LOGW(tag, "epoch after sync: %ld.%06ld", time.tv_sec, time.tv_usec);
}