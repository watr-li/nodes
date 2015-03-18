/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       microcoap example server endpoints
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 *
 * @}
 */

#include <stdbool.h>
#include <string.h>
#include "coap.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

const coap_endpoint_t endpoints[] =
{
    {(coap_method_t)0, NULL, NULL, NULL} /* marks the end of the endpoints array */
};
