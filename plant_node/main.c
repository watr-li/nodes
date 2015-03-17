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

#define ENABLE_DEBUG (0)
#include "debug.h"

int main(void)
{
    static unsigned int humidity;

    sensor_init();
    sensor_get_humidity(&humidity);
}




























