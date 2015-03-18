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

#include "sensor.h"
#include "vtimer.h"
#include "udp.h"
#include "rpl.h"
#include "coap.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

char* my_id;
char strbuf[6];
coap_endpoint_path_t register_path, humidity_path;

static radio_address_t set_watr_li_if(void);
int register_at_root(char *id);
int send_status_humidity(unsigned int *humidity);

int main(void)
{
    static unsigned int humidity;
    timex_t timer = timex_set(300, 0); /* seconds */

    radio_address_t ra_id = set_watr_li_if();

    /* stringify my_id. we'll be needing this in a sec. */
    sprintf(my_id, "%u", ra_id);
    /* Add my_id to humidity_path */
    register_path = (coap_endpoint_path_t) {1, {"nodes"}};
    humidity_path = (coap_endpoint_path_t) {3, {"nodes", my_id, "humidity"}};
    /* register my_id at the root node */
    register_at_root(my_id);

    sensor_init();
    while (1)
    {
        sensor_get_humidity(&humidity);
        send_status_humidity(&humidity);
        vtimer_sleep(timer);
    }
}

static radio_address_t set_watr_li_if(void)
{
    // TODO: fill with actual code
    return 23;
}

/**
 * @brief make this plant node known to the root node, identified by id.
 * @param[in]  id  The id tis node is known under
 */
int register_at_root(char *id)
{
    // TODO
    //coap_ext_build_PUT();
    return 0;
}

/**
 *
 */
int send_status_humidity(unsigned int *humidity)
{
    // TODO
    return 0;
}


























