/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       microcoap example server
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "udp.h"
#include "net_help.h"
#include "net_if.h"
#include "periph/cpuid.h" 
#include "board_uart0.h"
#include "thread.h"
#include "posix_io.h"
#include <coap.h>
#include "hashes.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define PORT 5683
#define BUFSZ 128

#define RCV_MSG_Q_SIZE      (64)

static void *_microcoap_server_thread(void *arg);

msg_t msg_q[RCV_MSG_Q_SIZE];
char _rcv_stack_buf[KERNEL_CONF_STACKSIZE_MAIN];

static ipv6_addr_t prefix;
int sock_rcv, if_id;
sockaddr6_t sa_rcv;
uint8_t buf[BUFSZ];
uint8_t scratch_raw[BUFSZ];
coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

static void _init_tlayer(void);
static uint16_t get_hw_addr(void);
int make_homemade_request(uint8_t* snd_buf);


int main(void)
{

    DEBUG("Starting example microcoap server...\n");

    _init_tlayer();
    thread_create(_rcv_stack_buf, KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN, CREATE_STACKTEST, _microcoap_server_thread, NULL ,"_microcoap_server_thread");

    DEBUG("Ready to receive requests.\n");

    return 0;
}

static uint16_t get_hw_addr(void)
{
    /* Use a hash of the cpu ID */
    /* 
    uint8_t cpuid[CPUID_ID_LEN];
    cpuid_get(cpuid);
    uint16_t hw_addr = (uint16_t) dek_hash(cpuid, CPUID_ID_LEN);
    DEBUG("Set hw addr to: %d\n", hw_addr);
    return hw_addr;
    */
    return 1;
}

/* init transport layer & routing stuff*/
static void _init_tlayer(void)
{
    msg_init_queue(msg_q, RCV_MSG_Q_SIZE);

    net_if_set_hardware_address(0, get_hw_addr());

    printf("initializing 6LoWPAN...\n");

    ipv6_addr_init(&prefix, 0xABCD, 0xEF12, 0, 0, 0, 0, 0, 0);
    if_id = 0; /* having more than one interface isn't supported anyway */

    sixlowpan_lowpan_init_interface(if_id);
}

static void *_microcoap_server_thread(void *arg)
{
    (void)arg; /* make the compiler shut up about unused variables */

    printf("initializing receive socket...\n");

    sa_rcv = (sockaddr6_t) { .sin6_family = AF_INET6,
               .sin6_port = HTONS(PORT) };

    sock_rcv = socket_base_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if (-1 == socket_base_bind(sock_rcv, &sa_rcv, sizeof(sa_rcv))) {
        printf("Error: bind to receive socket failed!\n");
        socket_base_close(sock_rcv);
    }

    printf("Ready to receive requests.\n");

    while(1)
    {
        int n, rc;
        socklen_t len = sizeof(sa_rcv);
        coap_packet_t pkt;

        n = socket_base_recvfrom(sock_rcv, buf, sizeof(buf), 0, &sa_rcv, &len);
        printf("Received packet: ");
        coap_dump(buf, n, true);
        printf("\n");

        if (0 != (rc = coap_parse(&pkt, buf, n)))
            printf("Bad packet rc=%d\n", rc);
        else
        {
            size_t rsplen = sizeof(buf);
            coap_packet_t rsppkt;
            printf("content:\n");
            coap_dumpPacket(&pkt);
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);

            if (0 != (rc = coap_build(buf, &rsplen, &rsppkt)))
                printf("coap_build failed rc=%d\n", rc);
            else
            {
                printf("Sending packet: ");
                coap_dump(buf, rsplen, true);
                printf("\n");
                printf("content:\n");
                coap_dumpPacket(&rsppkt);
                socket_base_sendto(sock_rcv, buf, rsplen, 0, &sa_rcv, sizeof(sa_rcv));

                printf("[main-posix] Following up with own initial GET:\n");
                
                // clear buffer
                memset(buf, 0, BUFSZ);

                if (0 == make_homemade_request(buf)) {
                    socket_base_sendto(sock_rcv, buf, rsplen, 0, &sa_rcv, sizeof(sa_rcv));
                    printf("[main-posix] homemade inital GET sent.\n");
                }
            }
        }
    }

    return NULL;
}

/* organic, local, gluten-free */
int make_homemade_request(uint8_t* snd_buf)
{
    printf("creating example GET request...\n");
    static char* msg= "hello";
    size_t req_pkt_sz;
    int errcode;

    // cobble together CoAP packet
    coap_header_t req_hdr = {
        .ver = 1,
        .t = COAP_TYPE_NONCON,
        .tkl = 0,                  /* microcoap can't generate tokens anyway */
        .code = MAKE_RSPCODE(0, COAP_METHOD_GET),  /* class 0, detail 1 */
        .id = {22,22}              /*let's see if this works :D */
    };

    coap_buffer_t payload = {
        .p = (const uint8_t *) msg,
        .len = strlen(msg)
    };

    coap_packet_t req_pkt = {
        .hdr = req_hdr,
        .tok = (coap_buffer_t) {}, /* No token */
        .numopts = 0,
        .opts = {},
        .payload = payload
    };

    req_pkt_sz = sizeof(req_pkt);

#ifdef DEBUG
    printf("[main-posix] content:\n");
    coap_dumpPacket(&req_pkt);
#endif

    // try to  write packet to send buffer
    if (0 != (errcode = coap_build(snd_buf, &req_pkt_sz, &req_pkt))) {
        printf("Error building packet! Error code: %i\nAborting. \n", errcode);
        return 1;
    }
    return 0;
}
