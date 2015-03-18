# About
This is a manual test application for the SEN0114 moisture sensor.

# Usage
This test application will initialize one ADC channel with 12 bit sampling depth and one GPIO which acts as power supply for the sensor, when a measure is taken.After initialization, the sensor value is read in an interval of some seconds and printed to the STDOUT.

To verify the seen value you can hold the sensor into a glass of water or touch both pins with your hands and see the values are changing.

# Notes
If using the sensor in your plant pot, you should not set the soil under continous voltage. Also you should not measure more often than "a couple  of times" in an hour.

This test uses timers. Be aware that RIOT timers in the current master may crach after an hour or so!

## Notes for samr21-xpro

You need to change the standard output device from `UART_0` to `UART_1` to free the external ADC reference voltage pin. Then you need to connect an external tty/USB converter to see outputs on your terminal (i.e.  pyterm). 

When using the samr21-xpro you need the ADC mode with external reference pin. When this is enabled in the periph.conf.h you need to connect the 3V3 pin with the V_ref pin (on samr21-xpro PA04).

# Needed pins summary

- GPIO Power pin - GPIO_0 - PA13
- ADC input pin  - ADC_0_POS_INPUT - PA06
- ADC ref. pin - ADC_0_REF_DEFAULT - PA04
- Serial out - UART_1_TX_PIN - PA22
- Serial int - UART_1_RX_PIN - PA23
