#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"

// PINS + CONSTANTS

// Inputs
#define DOOR_OCCUPANCY_PIN   GPIO_NUM_27      // PIR OUT
#define LIGHT_SENSOR_CH      ADC_CHANNEL_4   
#define ADC_ATTEN            ADC_ATTEN_DB_12
#define BITWIDTH             ADC_BITWIDTH_12

// Outputs
#define DOOR_LIGHT_PIN       GPIO_NUM_2
#define DOOR_SERVO_PIN       5

#define DARK_THRESHOLD_BITS  2000          
#define DOOR_OPEN_DELAY_MS   3000

// SERVO PWM 

#define SERVO_LEDC_TIMER     LEDC_TIMER_0
#define SERVO_LEDC_MODE      LEDC_LOW_SPEED_MODE
#define SERVO_LEDC_CHANNEL   LEDC_CHANNEL_0
#define SERVO_DUTY_RES       LEDC_TIMER_13_BIT
#define SERVO_FREQUENCY      50
#define SERVO_DUTY_CLOSE     230
#define SERVO_DUTY_OPEN      980

static void servo_ledc_init(void)
{
    ledc_timer_config_t tcfg = {
        .speed_mode       = SERVO_LEDC_MODE,
        .duty_resolution  = SERVO_DUTY_RES,
        .timer_num        = SERVO_LEDC_TIMER,
        .freq_hz          = SERVO_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&tcfg);

    ledc_channel_config_t ccfg = {
        .speed_mode = SERVO_LEDC_MODE,
        .channel    = SERVO_LEDC_CHANNEL,
        .timer_sel  = SERVO_LEDC_TIMER,
        .intr_type  = LEDC_INTR_DISABLE,
        .gpio_num   = DOOR_SERVO_PIN,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ccfg);
}

static void door_open(void)
{
    ledc_set_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL, SERVO_DUTY_OPEN);
    ledc_update_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL);
}

static void door_close(void)
{
    ledc_set_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL, SERVO_DUTY_CLOSE);
    ledc_update_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL);
}

// ADC (photoresistor)

static adc_oneshot_unit_handle_t adc1_handle;

static void light_sensor_adc_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = BITWIDTH
    };
    adc_oneshot_config_channel(adc1_handle, LIGHT_SENSOR_CH, &config);
}

static int light_sensor_read_bits(void)
{
    int bits = 0;
    adc_oneshot_read(adc1_handle, LIGHT_SENSOR_CH, &bits);
    return bits;
}

static bool is_dark(void)
{
    int bits = light_sensor_read_bits();
    printf("Light ADC bits = %d\n", bits);
}

//PIR interrupt
static volatile bool person_detected = false;
static void IRAM_ATTR door_occupancy_isr_handler(void *arg)
{
    person_detected = true;
}

void app_main(void)
{
    // Light output
    gpio_reset_pin(DOOR_LIGHT_PIN);
    gpio_set_direction(DOOR_LIGHT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DOOR_LIGHT_PIN, 0);

    // Servo init
    servo_ledc_init();
    door_close();

    // ADC init
    light_sensor_adc_init();

    // PIR setup
    gpio_reset_pin(DOOR_OCCUPANCY_PIN);
    gpio_set_direction(DOOR_OCCUPANCY_PIN, GPIO_MODE_INPUT);
    gpio_pullup_dis(DOOR_OCCUPANCY_PIN);
    gpio_pulldown_dis(DOOR_OCCUPANCY_PIN);

    gpio_set_intr_type(DOOR_OCCUPANCY_PIN, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(DOOR_OCCUPANCY_PIN, door_occupancy_isr_handler, NULL);
    gpio_intr_enable(DOOR_OCCUPANCY_PIN);

    bool door_is_busy = false;

    while (1) {

        if (person_detected && !door_is_busy) {

            person_detected = false;
            door_is_busy = true;

            //1) Open door
            door_open();

            //2) Check ambient light
            if (is_dark()) {
                gpio_set_level(DOOR_LIGHT_PIN, 1);
            } else {
                gpio_set_level(DOOR_LIGHT_PIN, 0);
            }
            // 3) Wait
            vTaskDelay(pdMS_TO_TICKS(DOOR_OPEN_DELAY_MS));
            // 4) Close door
            door_close();
            // 5) Light OFF when door closes
            gpio_set_level(DOOR_LIGHT_PIN, 0);
            door_is_busy = false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}