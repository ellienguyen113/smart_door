#include <stdio.h>
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

