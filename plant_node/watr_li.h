#ifndef WATR_LI_H
#define WATR_LI_H

#define WATR_LI_HYSTERESIS      (10)        /** < sensor value threshold */

#define WATR_LI_CHANNEL         (21)        /**< wireless channel */
#define WATR_LI_PAN             (0x03e9)    /**< The used PAN ID */
#define WATR_LI_IFACE           (0)         /**< trasmssion device */
#define WATR_LI_UDP_PORT        (5683)      /**< listen port, defaults to COAP */

#define WATR_LI_RECV_BUFLEN     (1024)      /**< recv buffer size */
#define WATR_LI_SEND_BUFLEN     (255)       /**< send buffer size */

#define WATR_LI_INIT_WAIT      (10)
#define WATR_LI_SEND_WAIT       (5)
#define WATR_LI_SENS_WAIT      (10)

#endif
