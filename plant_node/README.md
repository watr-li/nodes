# About
This is a manual test application for the SEN0114 moisture sensor.

# Usage
This test application will initialize one ADC channel with 12 bit sampling depth and one GPIO which acts as power supply for the sensor, when a measure is taken.After initialization, the sensor value is read in an interval of some seconds and printed to the STDOUT.

To verify the seen value you can hold the sensor into a glass of water or touch both pins with your hands and see the values are changing.

# Building

If you're building for the SAMR21 xpro you'll have to reduce the default number of RPL routing table entries, because the RAM of the hardware platform is very limited. This is done with the environment variable

    RPL_MAX_ROUTING_ENTRIES=4

So the full command for flashing onto the Samr board would look something like this:

    RPL_MAX_ROUTING_ENTRIES=4 BOARD=samr21-xpro make flash

# Notes
If using the sensor in your plant pot, you should not set the soil under continous voltage. Also you should not measure more often than "a couple  of times" in an hour.

This test uses timers. Be aware that RIOT timers in the current master may crach after an hour or so!
