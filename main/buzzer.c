#include "driver/ledc.h"

#define BUZZER_GPIO        18

#define BUZZER_TIMER       LEDC_TIMER_1
#define BUZZER_MODE        LEDC_LOW_SPEED_MODE
#define BUZZER_CHANNEL     LEDC_CHANNEL_1
#define BUZZER_DUTY_RES    LEDC_TIMER_10_BIT   

void buzzer_init(void){
    ledc_timer_config_t timer_cfg = {
        .speed_mode       = BUZZER_MODE,
        .duty_resolution  = BUZZER_DUTY_RES,
        .timer_num        = BUZZER_TIMER,
        .freq_hz          = 2000,              // default tone (2 kHz)
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_cfg);

    ledc_channel_config_t ch_cfg = {
        .gpio_num   = BUZZER_GPIO,
        .speed_mode = BUZZER_MODE,
        .channel    = BUZZER_CHANNEL,
        .timer_sel  = BUZZER_TIMER,
        .duty       = 0,                      
        .hpoint     = 0,
        .intr_type  = LEDC_INTR_DISABLE
    };
    ledc_channel_config(&ch_cfg);
}

void doorbell_sound()
{
    ledc_set_freq(LEDC_MODE, LEDC_TIMER, 2500);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 512);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

    vTaskDelay(pdMS_TO_TICKS(150));

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void alarm_sound()
{
    for(int i = 0; i < 3; i++)
    {
        ledc_set_freq(LEDC_MODE, LEDC_TIMER, 700);

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 512);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(300));

        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}