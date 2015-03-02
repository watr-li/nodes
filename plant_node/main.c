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

#include "shell.h"
#include "shell_commands.h"

#include "coap_ext.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define SND_PORT 5683
#define RCV_PORT 5684
#define BUFSZ 500

#define RCV_MSG_Q_SIZE      (64)

static void *_microcoap_server_thread(void *arg);

msg_t msg_q[RCV_MSG_Q_SIZE];
char _rcv_stack_buf[KERNEL_CONF_STACKSIZE_MAIN];

/* TODO: replace humidity with node ID */
static coap_endpoint_path_t path = {2, {"node", "humidity"}};

static ipv6_addr_t prefix;
int sock_snd, sock_rcv, if_id;
sockaddr6_t sa_snd, sa_rcv;
uint8_t buf[BUFSZ];
uint8_t scratch_raw[BUFSZ];
coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

static void _init_tlayer(void);
static uint16_t get_hw_addr(void);
int make_homemade_request(uint8_t* snd_buf);
static void _init_sock_snd(void);

static void send_put(int argc, char **argv)
{
    /* usage: put <payload> <ip> (!! PORT IS ASSUMED TO BE 5684)*/

    char* usage = "usage: put <payload> <ip> (!! PORT IS ASSUMED TO BE 5684)";

    if (argc != 3) {
        printf("%s\n",usage);
        return;
    }

    // clear buffer
    memset(buf, 0, BUFSZ);
    char* payload = argv[1];

    // turn <ip> into ipv6_addr_t
    inet_pton(AF_INET6, argv[2], &sa_snd.sin6_addr);
    sa_snd.sin6_family = AF_INET6;
    sa_snd.sin6_port = RCV_PORT;
    
    if (0 == coap_ext_build_PUT(buf, BUFSZ, payload, &path)) {
        socket_base_sendto(sock_snd, buf, strlen(buf), 0, &sa_snd, sizeof(sa_snd));
        printf("[main-posix] PUT with payload %s sent to %s:%i\n", argv[1], argv[2], sa_snd.sin6_port);
    }
}

const shell_command_t shell_commands[] = {
    {"put", "send put request", send_put},
    {NULL, NULL, NULL}
};

int main(void)
{
    DEBUG("Starting example microcoap server...\n");

    _init_tlayer();
    _init_sock_snd();
    thread_create(_rcv_stack_buf, KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN, CREATE_STACKTEST, _microcoap_server_thread, NULL ,"_microcoap_server_thread");

    /* Open the UART0 for the shell */
    posix_open(uart0_handler_pid, 0);

    printf("\n\t\t\tWelcome to RIOT\n\n");

    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);

    shell_run(&shell);

    DEBUG("Ready to receive requests.\n");

    return 0;
}


static uint16_t get_hw_addr(void)
{
    /* Use a hash of the cpu ID */
 
    uint8_t cpuid[CPUID_ID_LEN];
    cpuid_get(cpuid);
    uint16_t hw_addr = (uint16_t) dek_hash(cpuid, CPUID_ID_LEN);
    DEBUG("Set hw addr to: %d\n", hw_addr);
    return hw_addr;
    
    //return 1;
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

static void _init_sock_snd(void)
{
    printf("initializing send socket...\n");
    sa_snd = (sockaddr6_t) { .sin6_family = AF_INET6,
                             .sin6_port = HTONS(SND_PORT)};

    sock_snd = socket_base_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if(-1 == sock_snd) {
        printf("[demo]   Error Creating Socket!\n");
        return;
    }
}

static void *_microcoap_server_thread(void *arg)
{
    (void)arg; /* make the compiler shut up about unused variables */
    char *payload = "xoxo";

    printf("initializing receive socket...\n");

    sa_rcv = (sockaddr6_t) { .sin6_family = AF_INET6,
               .sin6_port = HTONS(RCV_PORT) };

    sock_rcv = socket_base_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if (-1 == (sock_rcv, &sa_rcv, sizeof(sa_rcv))) {
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

                printf("[main-posix] Following up with own initial PUT:\n");
                
                // clear buffer
                memset(buf, 0, BUFSZ);

                if (0 == coap_ext_build_PUT(buf, BUFSZ, payload, &path)) {
                    socket_base_sendto(sock_rcv, buf, rsplen, 0, &sa_rcv, sizeof(sa_rcv));
                    printf("[main-posix] inital PUT sent.\n");
                }
            }
        }
    }

    return NULL;
}
