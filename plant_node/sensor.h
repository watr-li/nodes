#ifndef SENSOR_H
#define SENSOR_H

#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "vtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"

#if ADC_NUMOF < 1
//#error "Please enable at least 1 ADC device to run this test"
#endif

#define RES                         ADC_RES_12BIT
#define ADC_IN_USE                  ADC_0
#define ADC_CHANNEL_USE             0
#define GPIO_POWER_PIN              GPIO_0

/**
 * @brief    Initialize humidity sensor.
 * @return   0 on success, 1 otherwise // TODO: proper error codes?
 */
int sensor_init(void);

/**
 * @brief Query the humidity sensor for data.
 * @param[in] humidity  int to which the humidity value should be written
 */
void sensor_get_humidity(unsigned int *humidity);

#endif /* SENSOR_H */