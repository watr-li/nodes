#ifndef NETWORK_H
#define NETWORK_H

/**
 * @brief prepares this node to join the watr.li DODAG
 *
 * @return 0 on success
 */
int watr_li_network_init(void);

/**
 * @brief initialize RPL on this watr.li node
 *
 * @return 0 on success
 */
int watr_li_init_rpl(void);

/**
 * @brief set IPv6 address of root node
 *
 * @param[in] root address string
 *
 * @return 0 on success, -1 otherwise
 */
int watr_li_set_root_addr (const char *root_addr_str);

/**
 * @brief make this plant node known to the root node, identified by id.
 *
 * @param[in]  id  The id tis node is known under
 *
 * @return 0 on success, -1 otherwise
 */
int watr_li_register_at_root(const char *id);

/**
 * @brief sends a packet to the DODAG ID (should be the root node IPv6 address)
 *
 * @param[in] payload pointer to the payload to be sent
 * @param[in] size number of bytes of the payload
 *
 * @return number of bytes send, -1 on error
 */
int watr_li_send(const char* payload, const size_t payload_size);

/**
 * @brief send the provided humidity value to the DODAG root.
 * @param[in]  humidity  pointer to the humidity value to send
 */
int watr_li_send_humidity(const int humidity);

/**
 * @brief create a thread to receive UDP messages
 */
void watr_li_start_server(void);

#endif
