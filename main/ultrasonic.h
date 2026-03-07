#ifndef HC_SR04_H
#define HC_SR04_H

#include "driver/gpio.h"

// Define Pins
#define TRIG_PIN GPIO_NUM_11
#define ECHO_PIN GPIO_NUM_12

// Function Prototypes
/*
 * @brief Initializes the GPIOs, Interrupts, and Hardware Timers for the sensor.
 */
void hc_sr04_init(void);

/*
 * @brief Triggers a measurement and returns the distance in centimeters.
 * @return float Distance in cm. Returns -1.0 if out of range or error.
 */
float hc_sr04_get_distance_cm(void);

#endif // HC_SR04_H