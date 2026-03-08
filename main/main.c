#include <stdio.h>
<<<<<<< HEAD
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Component APIs
#include "buzzer.h"
#include "keypad.h"
#include "ultrasonic.h"
#include "servo.h"
#include "ldr.h"
#include "light.h"

// --- SETTINGS ---
#define MODE_SWITCH_GPIO   GPIO_NUM_5   // Physical switch to toggle modes
char input_buffer[4] = {0, 0, 0, 0}; 
int input_count = 0;
char MASTER_PASSWORD[4] = {'7', '8', '9', '9'}; // The PIN
#define KEY_CLEAR '*'
#define NOPRESS '\0'


void run_door_flow(void) {
    printf("Opening Door...\n");
    buzzer_play_success_async(); // Activation tone
    door_open(); 
    if (ldr_is_dark()) {
        light_on();
    }
    else{
        light_off();
    }
    vTaskDelay(pdMS_TO_TICKS(5000)); // Stay open 5s
    door_close();
    light_off();
    printf("Door Closed.\n");
}

void app_main(void) {
    // 1. Initialize Components
    light_init();
    ldr_init(ADC_CHANNEL_9); 
    buzzer_init();
    init_keypad();
    ultrasonic_init();
    servo_init();

    // 2. Initialize the Mode Switch
    gpio_set_direction(MODE_SWITCH_GPIO, GPIO_MODE_INPUT);
    gpio_pullup_en(MODE_SWITCH_GPIO); // 1 = Auto, 0 = Keypad

    char input_buffer[4];
    int input_count = 0;

    printf("System Booted. Use Switch to Select Mode.\n");

    while (1) {
        // READ THE SWITCH: High (1) = Auto Mode, Low (0) = Keypad Mode
        int mode_select = gpio_get_level(MODE_SWITCH_GPIO);

        if (mode_select == 1) {
            // --- MODE 1: AUTO MODE (Ultrasonic) ---
            if (ultrasonic_get_distance_cm() < 30.0) {
                run_door_flow();
                
                // NEW: Wait until the person moves away before allowing another trigger
                printf("Waiting for person to leave...\n");
                while(ultrasonic_get_distance_cm() < 35.0) {
                    vTaskDelay(pdMS_TO_TICKS(100)); 
                }
                printf("Area clear. Monitoring resumed.\n");
            }
        }
        else {
            // --- MODE 3: KEYPAD MODE ---
            char key = get_key_buffered();
            if (key != NOPRESS) {
                if (key == KEY_CLEAR) {
                    input_count = 0;
                    printf("PIN Cleared.\n");
                } 
                else if ((key >= '0' && key <= '9')){
                    if (input_count<4){
                        input_buffer[input_count] = key;
                        input_count++;
                        printf("Digit %d/4\n", input_count);
                    }
                    if (input_count == 4) {
                        printf("DEBUG: Buffer is [%c%c%c%c]\n", 
                               input_buffer[0], input_buffer[1], input_buffer[2], input_buffer[3]);
                               
                        if (memcmp(input_buffer, MASTER_PASSWORD, 4) == 0) {
                            printf("Access Granted!\n");
                            run_door_flow();
                        } else {
                            printf("Access Denied!\n");
                            buzzer_play_failure_async(); // Warning tone
                        }
                        input_count = 0; // Reset for next try
                    }
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20)); 
    }
}
=======
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "buzzer.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "pir.h"
#include"ldr.h"
#include "light.h"
#include "keypad.h"
#include"buzzer.h"
#include "servo.h"
#include "bluetooth.h"

void app_main(void)
{
    // 1. Initialize the buzzer hardware
    printf("--- Initializing Buzzer Test ---\n");
    buzzer_init();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // 2. Test Success Melody (Blocking)
    // Your main task will stop here until the sound is finished.
    printf("1. Testing Success Melody (Blocking mode)...\n");
    play_success_opening();
    printf("Melody finished.\n\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    // 3. Test Failure Alarm (Blocking)
    printf("2. Testing Failure Alarm (Blocking mode)...\n");
    play_failure_alarm();
    printf("Alarm finished.\n\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    // 4. Test Asynchronous (Background) Play
    // This starts a NEW task. The print statement will show up 
    // IMMEDIATELY while the music is still playing.
    printf("3. Testing Success Melody (ASYNC mode)...\n");
    buzzer_play_success_async();
    
    for(int i = 0; i < 5; i++) {
        printf("   - Main task is running at the same time! (Step %d)\n", i);
        vTaskDelay(pdMS_TO_TICKS(150));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    // 5. CAUTION TEST: Overlapping without Mutex
    // Since you aren't using a mutex, calling these back-to-back 
    // will cause them to overlap and sound messy.
    printf("\n4. CAUTION: Testing overlap (No Mutex)...\n");
    printf("Expect garbled sound because tasks will fight for hardware!\n");
    buzzer_play_success_async();
    vTaskDelay(pdMS_TO_TICKS(50)); // Tiny delay
    buzzer_play_failure_async();

    printf("\n--- Buzzer Test Complete ---\n");
}

>>>>>>> 332e6a0c282f0212a4c34c8fa3b2d8734112cb9d
