APPLICATION = watr_li_node
#include ../Makefile.tests_common

BOARD ?= samr21-xpro
# #to avoid flooding the RAM
#RPL_MAX_ROUTING_ENTRIES = 4
RIOTBASE ?= $(CURDIR)/../../RIOT
FEATURES_REQUIRED = periph_adc periph_gpio
CFLAGS += -DDEVELHELP -DMICROCOAP_DEBUG -DCOAP_MAX_SEGMENTS=3

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netif_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_conn_udp
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_udp
USEMODULE += posix_sockets
USEMODULE += xtimer

USEPKG = microcoap

include $(RIOTBASE)/Makefile.include
