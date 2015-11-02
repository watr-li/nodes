#ifndef SENSOR_H
#define SENSOR_H

#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "xtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"

//#define WATR_LI_DUMMY_SENSOR (0)
#ifndef WATR_LI_DUMMY_SENSOR

#if ADC_NUMOF < 1
//#error "Please enable at least 1 ADC device to run this test"
#endif

/* NOTE: new PIN mapping!
 * Different from watr.li blog http://www.watr.li/Sensing-moisture.html
 */
#define ADC_NUM         0
#define ADC_CH          5 // OLD: 0
#define RES             ADC_RES_12BIT
#define DELAY           (100 * 1000U)

#define GPIO_VCC_PORT   1 // OLD: 0
#define GPIO_VCC_PIN    2 // OLD: 13

#endif
/**
 * @brief   Initialize humidity sensor.
 *
 * @return      0 on success, 1 otherwise // TODO: proper error codes?
 */
int watr_li_sensor_init(void);

/**
 * @brief   Query the humidity sensor for data.
 *
 * @param[in]   humidity  int to which the humidity value should be written
 */
void watr_li_get_humidity(int *humidity);

/**
 * @brief   Test if humidity change exeeds threshold
 *
 * @param[in]   prev_humidity
 * @param[in]   humidity, current value
 *
 * @return      true if significant, false otherwise
 */
bool watr_li_significant_humidity_change(const int prev_humidity, const int humidity);

#endif /* SENSOR_H */
