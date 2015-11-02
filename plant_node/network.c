/**
 * written by smlng
 */
// standard
 #include <inttypes.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
// network
 #include <arpa/inet.h>
 #include <netinet/in.h>
 #include <sys/socket.h>
 #include <unistd.h>
// riot
#include "board.h"
#include "periph/gpio.h"
#include "thread.h"

#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netapi.h"
#include "net/netopt.h"

#include "coap_ext.h"
#include "network.h"
#include "watr_li.h"

#include "debug.h"

/** The UDP server thread stack */
static const char watr_li_root_addr_str[] = "ff02::1";
static char watr_li_stack[THREAD_STACKSIZE_DEFAULT];

static int send_sock = -1;
static uint8_t send_buf[WATR_LI_SEND_BUFLEN];

struct sockaddr_in6 root_addr;

char my_id[32];
char strbuf[32];
coap_endpoint_path_t register_path, humidity_path;

int watr_li_network_init (void)
{
    DEBUG("%s()\n", __func__);
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    uint16_t channel = WATR_LI_CHANNEL;
    uint16_t pan_id = WATR_LI_PAN;

    if (0 >= gnrc_netif_get(ifs)) {
        puts ("[watr_li_network_init] ERROR: failed to get ifaces!");
        return -1;
    }
    if (0 > gnrc_netapi_set(ifs[0], NETOPT_CHANNEL, 0, (uint16_t *)&channel, sizeof(uint16_t))) {
        puts ("[watr_li_network_init] ERROR: failed to set channel!");
        return -1;
    }
    if (0 > gnrc_netapi_set(ifs[0], NETOPT_NID, 0, (uint16_t *)&pan_id, sizeof(uint16_t))) {
        puts ("[watr_li_network_init] ERROR: failed to set pan_id!");
        return -1;
    }

    uint8_t iid[8];
    if (0 > gnrc_netapi_get(ifs[0], NETOPT_IPV6_IID, 0, &iid, sizeof(iid))) {
        puts ("[watr_li_network_init] ERROR: failed to get IPv6 IID!");
        return -1;
    }

#ifdef WATR_LI_GLOBAL_IPV6
    ipv6_addr_t myaddr;
    ipv6_addr_set_aiid(&myaddr, iid);
    myaddr.u64[0] = byteorder_htonll(0x2015110700000000);
    if (0 > gnrc_ipv6_netif_add_addr(ifs[0], &myaddr, 64, 0)) {
        puts ("[watr_li_network_init] ERROR: failed to set IPv6 addr!");
        return -1;
    }
#endif

    send_sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (send_sock < 0) {
        puts("[watr_li_network_init] ERROR: initializing send socket!");
        return -1;
    }
    /* FIXME stringify my_id. we'll be needing this in a sec. */
    memset(my_id, 0, sizeof(my_id));
    sprintf(my_id,
            "%02X%02X%02X%02X%02X%02X%02X%02X",
            iid[0],iid[1],iid[2],iid[3],iid[4],iid[5],iid[6],iid[7]);

    /* Add my_id to humidity_path */
    register_path = (coap_endpoint_path_t) {1, {"nodes"}}; //FIXME: should be nodes/my_id ?
    humidity_path = (coap_endpoint_path_t) {3, {"nodes", my_id, "humidity"}};

    if (0 > watr_li_set_root_addr(watr_li_root_addr_str)) {
        puts("[watr_li_network_init] ERROR: failed to set root_addr!");
        return -1;
    }
    if (0 > watr_li_register_at_root(my_id)) {
        puts("[watr_li_network_init] ERROR: failed to register at root!");
        return -1;
    }
    return 0;
}

int watr_li_set_root_addr (const char *root_addr_str)
{
    /* parse port */
    uint16_t port = (uint16_t)WATR_LI_UDP_PORT;
    if (port == 0) {
        puts("[watr_li_set_root] Error: invalid port specified");
        return -1;
    }
    root_addr.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, root_addr_str, &root_addr.sin6_addr) != 1) {
        puts("[watr_li_set_root] Error: unable to parse root address");
        return -1;
    }
    root_addr.sin6_port = htons(port);
    return 0;
}

int watr_li_register_at_root (const char *id)
{
    DEBUG("%s()\n", __func__);

    size_t send_buflen = WATR_LI_SEND_BUFLEN;
    /* FIXME: correct REST would require a POST here */
    if (0 == coap_ext_build_PUT(send_buf, &send_buflen, id, &register_path)) {
        /* TODO: check if sent successfully, else wait & retry */
        watr_li_send((char*) send_buf, send_buflen);
        DEBUG("[watr_li_register_at_root] successfully registered with id %s\n", *register_path.elems);
        return 0;
    }
    DEBUG("[watr_li_register_at_root] failed to register with with id %s\n", *register_path.elems);
    return -1;
}

int watr_li_send (const char* payload, const size_t payload_size)
{
    DEBUG("%s()\n", __func__);

    int bytes_sent;

    if (0 > send_sock) {
        puts("[watr_li_send] ERROR: invalid socket!");
        return -1;
    }

    if (0 > ( bytes_sent=sendto(send_sock, payload, payload_size, 0,
                                (struct sockaddr *)&root_addr, sizeof(root_addr)))
        ) {
        puts("[watr_li_send] Error sending packet!");
        return -1;
    }
    printf("[watr_li_send] delivered %i bytes to the root node.\n", bytes_sent);
    return bytes_sent;
}

/**
 * @brief send the provided humidity value to the DODAG root.
 * @param[in]  humidity  pointer to the humidity value to send
 */
int watr_li_send_humidity (const int humidity)
{
    DEBUG("%s()\n", __func__);

    /* stringify humidity */
    sprintf(strbuf, "%d", humidity);
    size_t send_buflen = WATR_LI_SEND_BUFLEN;
    /* Clear buffer */
    memset(send_buf,0, send_buflen);



    if (0 != coap_ext_build_PUT(send_buf, &send_buflen, strbuf, &humidity_path)) {
        DEBUG("[watr_li_send_humidity] failed to send humidity \n");
        return -1;
    }
    /* TODO: check if sent successfully, else wait & retry */
    if (0 > watr_li_send((char*) send_buf, send_buflen)) {
        puts("[watr_li_send_humidity] ERROR: failed to send sensor data!");
        return -1;
    }
    printf("[watr_li_send_humidity] successfully sent humidity value: %s\n", strbuf);
    return 0;
}

/* watr.li server */

/**
* @brief the sample UDP server that expects receiving strings
* @param[in] arg unused parameter pointer
*/
static void *watr_li_server (void *arg)
{
    DEBUG("%s()\n", __func__);

    (void) arg;

    struct sockaddr_in6 server_addr;
    char src_addr_str[IPV6_ADDR_MAX_STR_LEN];
    char recv_buf[WATR_LI_RECV_BUFLEN];
    uint16_t port;
    int sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        puts("error initializing socket");
        return NULL;
    }
    /* parse port */
    port = (uint16_t)WATR_LI_UDP_PORT;
    if (port == 0) {
        puts("Error: invalid port specified");
        return NULL;
    }
    server_addr.sin6_family = AF_INET6;
    memset(&server_addr.sin6_addr, 0, sizeof(server_addr.sin6_addr));
    server_addr.sin6_port = htons(port);

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        puts("error binding socket");
        return NULL;
    }
    printf("Success: started watr.li server on port %" PRIu16 "\n", port);

    while (1) {
        int res;
        struct sockaddr_in6 src;
        socklen_t src_len = sizeof(struct sockaddr_in6);
        // blocking receive, waiting for data
        if ((res = recvfrom(sock, recv_buf, sizeof(recv_buf), 0,
                            (struct sockaddr *)&src, &src_len)) < 0) {
            puts("[watr_li_udp_server] ERROR: recsize < 0!");
        }
        else if (res == 0) {
            puts("[watr_li_udp_server] WARN: Peer did shut down");
        }
        else { // check for PING or PONG
            inet_ntop(AF_INET6, &(src.sin6_addr),
                      src_addr_str, sizeof(src_addr_str));
            /* if we received a string print it */
            if (recv_buf[res-1] == '\0' ) {
                printf("UDP packet received, payload:\n%s\n", recv_buf);
            } else {
                /* print the buffer bytes in hex */
                printf("UDP packet received, payload (%d bytes):\n", (int)res);
                for(int i = 0; i < res; ++i) {

                    if ( (i%8) == 0 ) {
                        /* newline after 8 bytes */
                        puts("");
                    }

                    printf("%02x ", recv_buf[i]);
                }
                puts("");
            }
        }

    }

    return NULL;
}

/**
* @brief create a thread to receive UDP messages
*/
void watr_li_start_server (void)
{
    DEBUG("%s()\n", __func__);
    thread_create(watr_li_stack, sizeof(watr_li_stack), THREAD_PRIORITY_MAIN,
                     CREATE_STACKTEST, watr_li_server, NULL, "watr.li server");
}
