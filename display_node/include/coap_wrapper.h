#ifndef _COAP_WRAPPER_H_
#define _COAP_WRAPPER_H_

#include <stdlib.h>
#include <stdio.h>

#include "coap_list.h"
#include "pdu.h"
#include "option.h"
#include "net.h"
#include "block.h"

#include "mutex.h"
#include "debug.h"

#define BUFSIZE          40
#define FLAGS_BLOCK      0x01
#define NI_MAXSERV      32 /* according to the getnameinfo manpage. */

typedef unsigned char coap_opt_t;

int coap_wrapper_init(int* sock_ptr, mutex_t* sock_mutex_ptr);
int coap_wrapper_post(char* resource);

#endif /* _COAP_WRAPPER_H_ */
