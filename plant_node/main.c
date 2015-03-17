/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for the SEN0114 moisture sensor
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "vtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if ADC_NUMOF < 1
//#error "Please enable at least 1 ADC device to run this test"
#endif

#define RES                         ADC_RES_12BIT
#define ADC_IN_USE                  ADC_0
#define ADC_CHANNEL_USE             0
#define GPIO_POWER_PIN              GPIO_0

timex_t sleep_timer = timex_set(1, 0); /* 1 sec. */

int main(void)
{
    static unsigned int humidity;

    init_sensor_humidity();
    sense_humidity(&humidity);
}


/**
 * @brief    Initialize humidity sensor.
 * @return   0 on success, 1 otherwise // TODO: proper error codes?
 */
int init_sensor_humidity(void)
{

    /* initialize a GPIO that powers the sensor just during a measure */
    DEBUG("Initializing GPIO_%i as power supplying pin", GPIO_POWER_PIN);
    if (gpio_init_out(GPIO_POWER_PIN, GPIO_NOPULL) == 0) {
        DEBUG("    [ok]\n");
    }
    else {
        DEBUG("    [failed]\n");
        return 1;
    }

    /* initialize ADC device */
    DEBUG("Initializing ADC_%i @ %i bit resolution", ADC_IN_USE, (6 + (2* RES)));
    if (adc_init(ADC_IN_USE, RES) == 0) {
        DEBUG("    [ok]\n");
    }
    else {
        DEBUG("    [failed]\n");
        return 1;
    }
}

/**
 * @brief Query the humidity sensor for data.
 * @param[in] humidity  int to which the humidity value should be written
 */
void sense_humidity(unsigned int *humidity)
{
    DEBUG("Checking moisture...\n");

    gpio_set(GPIO_POWER_PIN);

    /* wait until the sensor is ready to go */
    vtimer_sleep(sleep_timer);
    *humidity = adc_sample(ADC_IN_USE, ADC_CHANNEL_USE);
    gpio_clear(GPIO_POWER_PIN);
    printf("Humidity: ADC_%i: %4i\n", ADC_IN_USE, *humidity);
}






























