APPLICATION = plant_node
#include ../Makefile.tests_common

BOARD ?= samr21-xpro
RIOTBASE ?= $(CURDIR)/../../RIOT
FEATURES_REQUIRED = periph_adc
CFLAGS += -DDEVELHELP -DMICROCOAP_DEBUG

USEPKG = microcoap

USEMODULE += vtimer
USEMODULE += rpl
USEMODULE += udp

include $(RIOTBASE)/Makefile.include
