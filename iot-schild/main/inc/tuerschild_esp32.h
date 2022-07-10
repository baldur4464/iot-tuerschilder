#ifndef TUERSCHILD_ESP32_H
#define TUERSCHILD_ESP32_H
/***************************************
 * platform dependent defines
 *  TUERSCHILD_LOGW log with level warning
 *  TUERSCHILD_LOGI log with level info
 *  TUERSCHILD_LOGE log with level error
 ***************************************/
#include "esp_log.h"

#define TUERSCHILD_LOGW(args...) ESP_LOGW(args)
#define TUERSCHILD_LOGI(args...) ESP_LOGI(args)
#define TUERSCHILD_LOGE(args...) ESP_LOGE(args)

#endif
