#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pir.h"
#include"ldr.h"
#include "light.h"
#include "keypad.h"
#include"buzzer.h"
#include "servo.h"

#define NUM_DOORS 3
#define LOOP_MS 50
#define DOOR_OPEN_TIME_MS 2500
#define PIR_LOCKOUT_MS 1200 //prevent repeated triggers
#define LDR_DARK_THRESHOLD 1800
#define PIN_LEN 4

typedef enum{
    MODE_AUTO=0, 
    MODE_REMOTE=1, 
    MODE_KEYPAD=2
} mode_t;

//For testing (BLE is not ready)
static int selected_door = 1;
static mode_t selected_mode = MODE_AUTO;

typedef enum { 
    CMD_NONE=0, 
    CMD_OPEN=1, 
    CMD_CLOSE=2 
} remote_cmd_t;
static remote_cmd_t remote_cmd = CMD_NONE;

static const char DOOR_PIN[NUM_DOORS][PIN_LEN+1]={
    "1111",
    "2222",
    "3333"
};

static int clamp_door(int d){
    if (d < 1){
        return 1;
    } 
    if (d > NUM_DOORS){
        return NUM_DOORS;
    }
}return d;

static bool is_dark(void)
{
    int raw = ldr_read_raw();
    return (raw >= LDR_DARK_THRESHOLD);
}

static void update_light (bool door_opened, bool person_nearby)
{
    // If dark AND (person nearby OR door opens) -> light ON
    if (is_dark() && (door_opened || person_nearby)){
        light_on();
    }
    else {
        light_off();
    }
}
static void open_then_auto_close(int door)
{
    door_open(door);
    doorbell_sound();  

    update_light(true, true);

    vTaskDelay(pdMS_TO_TICKS(DOOR_OPEN_TIME_MS));

    door_close(door);
    light_off();
}
// Keypad PIN entry

static bool read_pin_from_keypad(char *out_pin /*size PIN_LEN+1*/)
{
    int idx = 0;
    memset(out_pin, 0, PIN_LEN + 1);

    // Collect exactly PIN_LEN digits; '#' submits, '*' clears.
    while (1) {
        char k = scan_keypad();
        if (k == NOPRESS) {
            vTaskDelay(pdMS_TO_TICKS(LOOP_MS));
            continue;
        }
        if (k == '*') {
            idx = 0;
            memset(out_pin, 0, PIN_LEN + 1);
            alarm_sound(); // feedback optional
        }
        else if (k == '#') {
            if (idx == PIN_LEN) return true;   // PIN ready
            // not enough digits
            alarm_sound();
            idx = 0;
            memset(out_pin, 0, PIN_LEN + 1);
        }
        else if (k >= '0' && k <= '9') {
            if (idx < PIN_LEN) {
                out_pin[idx++] = k;
                // optional tiny beep
                // doorbell_sound();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(LOOP_MS));
    }
}

void app_main(void)
{
    // Init modules
    pir_init();
    init_keypad();
    buzzer_init();
    light_init();

    // LDR init
    ldr_init(ADC_CHANNEL_2);

    // Multi-door servo init
    servo_init_all();

    // Safe startup state
    for (int d = 1; d <= NUM_DOORS; d++) {
        door_close(d);
    }light_off();
    
    char entered[PIN_LEN + 1];

    while (1) {
        int door = clamp_door(selected_door);

        if (selected_mode == MODE_AUTO) {
            // PIR controls opening
            if (pir_read()) {
                open_then_close(door, true);
                vTaskDelay(pdMS_TO_TICKS(PIR_LOCKOUT_MS));
            } else {
                light_off();
            }
        }

        else if (selected_mode == MODE_REMOTE) {
            // Simulate remote open/close commands
            if (remote_cmd == CMD_OPEN) {
                door_open(door);
                doorbell_sound();
                update_light(true, false);
                remote_cmd = CMD_NONE;
            }
            else if (remote_cmd == CMD_CLOSE) {
                door_close(door);
                light_off();
                remote_cmd = CMD_NONE;
            }
        }

        else if (selected_mode == MODE_KEYPAD) {
            // Keypad only for PIN (door selection done elsewhere)
            bool got_pin = read_pin_from_keypad(entered);
            if (got_pin) {
                if (strcmp(entered, DOOR_PIN[door - 1]) == 0) {
                    open_then_close(door, false);
                } else {
                    alarm_sound();
                    door_close(door);
                    light_off();
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(LOOP_MS));
    }
}