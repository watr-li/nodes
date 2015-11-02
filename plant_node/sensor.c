#include "sensor.h"
#include "watr_li.h"
#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef WATR_LI_DUMMY_SENSOR
int watr_li_sensor_init(void)
{
    /* initialize a GPIO that powers the sensor just during a measure */
    DEBUG("Initializing GPIO_%i as power supplying pin", GPIO_VCC_PIN);
    /* initialize GPIO pin as power supply */
    gpio_init(GPIO_PIN(GPIO_VCC_PORT, GPIO_VCC_PIN), GPIO_DIR_OUT, 0);

    /* initialize ADC device */
    DEBUG("Initializing ADC @ %i bit resolution", (6 + (2* RES)));
    if (adc_init(ADC_NUM, RES) == 0) {
        DEBUG("    [ok]\n");
    }
    else {
        DEBUG("    [failed]\n");
        return 1;
    }
    return 0;
}

void watr_li_get_humidity(int *humidity)
{
    DEBUG("Checking moisture...\n");

    gpio_set(GPIO_PIN(GPIO_VCC_PORT, GPIO_VCC_PIN));

    /* wait until the sensor is ready to go */
    xtimer_sleep(WATR_LI_INIT_WAIT);
    *humidity = adc_sample(ADC_NUM, ADC_CH);
    gpio_clear(GPIO_PIN(GPIO_VCC_PORT, GPIO_VCC_PIN));
    printf("ADC_%i-CH%i: %4i \n", ADC_NUM, ADC_CH, *humidity);
}

#else
int watr_li_sensor_init(void)
{
    return 0;
}

void watr_li_get_humidity(int *humidity)
{
    int tmp = *humidity;
    if (tmp % 2 == 0) {
        *humidity = tmp/2 + 1;
    }
    else {
        *humidity = (3*tmp) - 1;
    }
    printf("[watr_li_get_humidity] humidity %d.\n", *humidity);
}
#endif

/**
 * @brief decide if the change in humidity is not just a slight variation.
 */
bool watr_li_significant_humidity_change(const int prev_humidity, const int humidity)
{
    if (prev_humidity > humidity) { // humidity decreases
        return ((humidity < (prev_humidity-WATR_LI_HYSTERESIS)) ? true : false);
    }
    // else, humidity increases
    return ((humidity > (prev_humidity+WATR_LI_HYSTERESIS)) ? true : false);
}
