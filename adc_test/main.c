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
 *
 * @}
 */

#include <stdio.h>
#include <math.h>

#include "cpu.h"
#include "board.h"
#include "vtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"

#if ADC_NUMOF < 1
#error "Please enable at least 1 ADC device to run this test"
#endif

#define RES                         ADC_RES_12BIT
#define ADC_IN_USE                  ADC_0
#define ADC_CHANNEL_USE             0
#define GPIO_POWER_PIN              GPIO_0

static unsigned int value;

int main(void)
{
    puts("\nRIOT test for a moisture sensor\n");

    timex_t sleep1 = timex_set(1, 0); /* 1 sec. */
    timex_t sleep2 = timex_set(1, 0); /* 1 sec. */

    /* initialize a GPIO that powers the sensor just during a measure */
    printf("Initializing GPIO_%i as power supplying pin", GPIO_POWER_PIN);
    if (gpio_init_out(GPIO_POWER_PIN, GPIO_NOPULL) == 0) {
        puts("    ...[ok]");
    }
    else {
        puts("    ...[failed]");
        return 1;
    }
    puts("\n");

    /* initialize ADC device */
    printf("Initializing ADC_%i @ %i bit resolution", ADC_IN_USE, (6 + (2* RES)));
    if (adc_init(ADC_IN_USE, RES) == 0) {
        puts("    ...[ok]");
    }
    else {
        puts("    ...[failed]");
        return 1;
    }
    puts("\n");

    while (1) {

        gpio_set(GPIO_POWER_PIN);

        /* just for safety */
        vtimer_sleep(sleep1);

        value = adc_sample(ADC_IN_USE, ADC_CHANNEL_USE);

        gpio_clear(GPIO_POWER_PIN);

        /* print the result */
        printf("Value: ADC_%i: %4i\n", ADC_IN_USE, value);

        if (value >= 2000) {
            puts("Soil is wet");
        }
        else if (value > 1000) {
            puts("Soil is normal");
        }
        else if(value < 1000) {
            puts("Soil is dry");
        }

         /* wait for next measure */
        vtimer_sleep(sleep2);
    }

    return 0;
}
