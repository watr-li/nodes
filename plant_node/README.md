# About
This is a manual test application for the SEN0114 moisture sensor.

# Usage
This test application will initialize one ADC channel with 12 bit sampling depth and one GPIO which acts as power supply for the sensor, when a measure is taken.After initialization, the sensor value is read in an interval of some seconds and printed to the STDOUT.

To verify the seen value you can hold the sensor into a glass of water or touch both pins with your hands and see the values are changing.

# Notes
If using the sensor in your plant pot, you should not set the soil under continous voltage. Also you should not measure more often than "a couple  of times" in an hour.

This test uses timers. Be aware that RIOT timers in the current master may crach after an hour or so!