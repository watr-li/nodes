#include "sensor.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

timex_t sleep_timer;

int sensor_init(void)
{
    sleep_timer = timex_set(1, 0); /* 1 sec. */

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
    return 0;
}

void sensor_get_humidity(unsigned int *humidity)
{
    DEBUG("Checking moisture...\n");

    gpio_set(GPIO_POWER_PIN);

    /* wait until the sensor is ready to go */
    vtimer_sleep(sleep_timer);
    *humidity = adc_sample(ADC_IN_USE, ADC_CHANNEL_USE);
    gpio_clear(GPIO_POWER_PIN);
    printf("Humidity: ADC_%i: %4i\n", ADC_IN_USE, *humidity);
}

