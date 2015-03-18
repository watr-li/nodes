APPLICATION = plant_node
#include ../Makefile.tests_common

BOARD ?= samr21-xpro
# #to avoid flooding the RAM
#RPL_MAX_ROUTING_ENTRIES = 4
RIOTBASE ?= $(CURDIR)/../../RIOT
FEATURES_REQUIRED = periph_adc
CFLAGS += -DDEVELHELP -DMICROCOAP_DEBUG

USEPKG = microcoap

USEMODULE += vtimer
USEMODULE += rpl
USEMODULE += udp
USEMODULE += defaulttransceiver

include $(RIOTBASE)/Makefile.include
