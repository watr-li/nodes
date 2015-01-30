/* PLEASE NOTE: This code has been cobbled together from the
 *
 * coap-client -- simple CoAP client
 *
 * Copyright (C) 2010--2013 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use. 
 * 
 * And extended by Lote Steenbrink <lotte.steenbrink@haw-hamburg.de> in a desperate
 * attempt to create an API which one can use without knowing the internals of 
 * libcoap.
 */

#include "include/coap_wrapper.h"

static int* _sock_ptr;
static mutex_t* _sock_mutex_ptr;

/* client things */
unsigned char msgtype = COAP_MESSAGE_CON; /* usually, requests are sent confirmable */
static str payload = { 0, NULL }; /* optional payload to send */
static unsigned char _token_data[8];
str the_token = { 0, _token_data };
int flags = 0;
char port_str[NI_MAXSERV] = "0";
coap_block_t block = { .num = 0, .m = 0, .szx = 6 };
coap_pdu_t  *pdu;

coap_list_t* new_option_node(unsigned short key, unsigned int length, unsigned char *data);
int order_opts(void *a, void *b);
coap_pdu_t * coap_new_request(coap_context_t *ctx, unsigned char m, coap_list_t *options);

int coap_wrapper_init(int* sock_ptr, mutex_t* sock_mutex_ptr) {
    // TODO
    _sock_ptr = sock_ptr;
    _sock_mutex_ptr = sock_mutex_ptr;

    return 0;
}

/* Super simple POST for demo purposes.*/
int coap_wrapper_post(char* resource) {

    static coap_list_t *optlist = NULL;
    coap_opt_t *buf[BUFSIZE];

    coap_insert(&optlist, new_option_node(COAP_OPTION_URI_PATH,
                          COAP_OPT_LENGTH(buf),
                          COAP_OPT_VALUE(buf)),
                          order_opts);

    
    /* create context for IPv6 (RIOT doesn't support IPv4, so no check necessary) */
    coap_context_t *ctx = get_context("::", port_str);
    
    if (! (pdu = coap_new_request(ctx, "post", optlist))) {
            return -1;
    }

    // TODO actually send

    return 0;
}

// ==== CLIENT HELPER CODE STARTS HERE =========================================

coap_list_t *
new_option_node(unsigned short key, unsigned int length, unsigned char *data) {
    coap_option *option;
    coap_list_t *node;

    option = malloc(sizeof(coap_option) + length);
    if ( !option ) {
        printf("new_option_node: malloc\n");
        free(option);
        return NULL;
    }
    COAP_OPTION_KEY(*option) = key;
    COAP_OPTION_LENGTH(*option) = length;
    memcpy(COAP_OPTION_DATA(*option), data, length);

    /* we can pass NULL here as delete function since option is released automatically  */
    node = coap_new_listnode(option, NULL);

    if (node) {
        return node;
    }
}

int
order_opts(void *a, void *b) {
  if (!a || !b)
    return a < b ? -1 : 1;

  if (COAP_OPTION_KEY(*(coap_option *)a) < COAP_OPTION_KEY(*(coap_option *)b))
    return -1;

  return COAP_OPTION_KEY(*(coap_option *)a) == COAP_OPTION_KEY(*(coap_option *)b);
}

coap_pdu_t *
coap_new_request(coap_context_t *ctx, unsigned char m, coap_list_t *options ) {
  coap_pdu_t *pdu;
  coap_list_t *opt;

  if ( ! ( pdu = coap_new_pdu() ) )
    return NULL;

  pdu->hdr->type = msgtype;
  pdu->hdr->id = coap_new_message_id(ctx);
  pdu->hdr->code = m;

  pdu->hdr->token_length = the_token.length;
  if ( !coap_add_token(pdu, the_token.length, the_token.s)) {
    DEBUG("cannot add token to request\n");
  }

  //coap_show_pdu(pdu);

  for (opt = options; opt; opt = opt->next) {
    coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option *)opt->data),
            COAP_OPTION_LENGTH(*(coap_option *)opt->data),
            COAP_OPTION_DATA(*(coap_option *)opt->data));
  }

  if (payload.length) {
    if ((flags & FLAGS_BLOCK) == 0)
      coap_add_data(pdu, payload.length, payload.s);
    else
      coap_add_block(pdu, payload.length, payload.s, block.num, block.szx);
  }

  return pdu;
}
