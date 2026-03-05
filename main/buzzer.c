#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"

#define BUZZER_GPIO        4
#define BUZZER_TIMER       LEDC_TIMER_1
#define BUZZER_MODE        LEDC_LOW_SPEED_MODE
#define BUZZER_CHANNEL     LEDC_CHANNEL_1
#define BUZZER_DUTY_RES    LEDC_TIMER_13_BIT


#define BUZZER_DUTY_ON     218
#define BUZZER_DUTY_OFF    981

void buzzer_init(void)
{
    ledc_timer_config_t timer_cfg = {
        .speed_mode       = BUZZER_MODE,
        .duty_resolution  = BUZZER_DUTY_RES,
        .timer_num        = BUZZER_TIMER,
        .freq_hz          = 2000,              // default tone (2 kHz)
        .clk_cfg          = LEDC_AUTO_CLK
    };
    (void)ledc_timer_config(&timer_cfg);

    ledc_channel_config_t ch_cfg = {
        .gpio_num   = BUZZER_GPIO,
        .speed_mode = BUZZER_MODE,
        .channel    = BUZZER_CHANNEL,
        .timer_sel  = BUZZER_TIMER,
        .duty       = BUZZER_DUTY_OFF,
        .hpoint     = 0,
        .intr_type  = LEDC_INTR_DISABLE
    };
    (void)ledc_channel_config(&ch_cfg);

    // Ensure silent at init
    ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, BUZZER_DUTY_OFF);
    ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
}

void doorbell_sound()
{
    ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, 2500);

    ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, BUZZER_DUTY_ON);
    ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);

    vTaskDelay(pdMS_TO_TICKS(150));

    ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, BUZZER_DUTY_OFF);
    ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
}

void alarm_sound()
{
    ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, 700);

    ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, BUZZER_DUTY_ON);
    ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);

    vTaskDelay(pdMS_TO_TICKS(300));

    ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, BUZZER_DUTY_OFF);
    ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);

    vTaskDelay(pdMS_TO_TICKS(200));
}