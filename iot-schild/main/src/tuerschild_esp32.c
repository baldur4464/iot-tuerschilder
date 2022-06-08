#include "tuerschild.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_check.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "nvs_flash.h"

static const char* tag= "tuerschild_esp32";

int early_init()
{
	int ret = 1;
	esp_err_t error;
	
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

int config_requested()
{
	//TODO implement
	TUERSCHILD_LOGW(tag, "using dummy function 'config_requested'");
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
	//TODO implement
	TUERSCHILD_LOGW(tag, "dummy yield function");
	vTaskDelay(100/portTICK_PERIOD_MS);
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
	//TODO implement
	TUERSCHILD_LOGW(tag, "dummy sleep function");
	esp_deep_sleep(20*1000000);
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

