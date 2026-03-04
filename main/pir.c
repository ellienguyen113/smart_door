#include "pir.h"
#include "driver/gpio.h"

#define PIR_GPIO GPIO_NUM_4

void pir_init(void)
{
    gpio_config_t pir_config = {
        .pin_bit_mask = (1ULL << PIR_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&pir_config);
}

int pir_read(void)
{
    return gpio_get_level(PIR_GPIO);
}