/*********************************************************
 * component for reading user input on the config-request-button
 *  init_reset_btn      init the button as input
 *  reset_btn_pressed   true if button is pressed, false if not
 ********************************************************/

static const char* tag = "tuerschild_btn_esp32";

#include "tuerschild.h"
#include "driver/gpio.h"



#define RESET_PIN GPIO_NUM_18
#define RESET_PIN_MSK (1<<RESET_PIN)

static const gpio_config_t pin_cfg = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_INPUT,
    .pin_bit_mask = RESET_PIN_MSK,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_ENABLE    //button is pulldown
};

int init_reset_btn()
{
    esp_err_t err = gpio_config(&pin_cfg);
    if(err!= ESP_OK) {
        TUERSCHILD_LOGE(tag, "GPIO init failed");
        return 0;
    } else {
        return 1;
    }
    
}

int reset_btn_pressed()
{
    return !gpio_get_level(RESET_PIN);  //button is pull down
}