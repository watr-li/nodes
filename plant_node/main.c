/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/* standard includes */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* watr.li includes */
#include "network.h"
#include "sensor.h"
#include "watr_li.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

int main(void)
{
    int humidity, prev_humidity;

    DEBUG("Setting up watr.li app...\n");
    watr_li_network_init(); /* initialize network */
    watr_li_sensor_init();  /* initialize humidity sensor */

    xtimer_sleep(WATR_LI_INIT_WAIT);

    /* get initial sensor value */
    watr_li_get_humidity(&humidity);
    prev_humidity = humidity;

    DEBUG("...Done. Bring it on, plants!\n");
    while (1)
    {
        /* get and eval humidity */
        watr_li_get_humidity(&humidity);
        if (watr_li_significant_humidity_change(prev_humidity, humidity)){
            /* humidity changed enough, send new value */
            if (watr_li_send_humidity(humidity) == 0) {
                /* success sending, update prev_humidity */
                prev_humidity = humidity;
            }
        }
        /* wait */
        xtimer_sleep(WATR_LI_SENS_WAIT);
    }
    /* never get here */
    return 0;
}
