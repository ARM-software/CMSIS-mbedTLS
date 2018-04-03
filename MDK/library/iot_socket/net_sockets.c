/*
 *  TCP/IP or UDP/IP networking functions for MDK-Pro Network Dual Stack
 *
 *  Copyright (C) 2006-2018, Arm Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
 #include "mbedtls/config.h"
#else
 #include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_NET_C)

#include <stdio.h>
#include <string.h>
#include "RTE_Components.h"
#include "mbedtls/net_sockets.h"
#include "iot_socket.h"
#if defined(RTE_CMSIS_RTOS)
 #include "cmsis_os.h"
#elif defined(RTE_CMSIS_RTOS2)
 #include "cmsis_os2.h"
#else
 #error "::CMSIS:RTOS selection invalid"
#endif

#undef MBEDTLS_NET_LISTEN_BACKLOG
#define MBEDTLS_NET_LISTEN_BACKLOG      3

#if !defined(RTE_Network_IPv6)
 #define IP_LEN     4
#else
 #define IP_LEN     16
#endif

/*
 * Initialize a context
 */
void mbedtls_net_init (mbedtls_net_context *ctx) {
  ctx->fd = -1;
}

/*
 * Initiate a TCP connection with host:port and the given protocol
 */
int mbedtls_net_connect (mbedtls_net_context *ctx, const char *host, const char *port, int proto) {
  uint8_t  ip_addr[IP_LEN];
  uint32_t ip_len = IP_LEN;
  uint16_t port_nr;
  int32_t  af, ret;

  /* Do name resolution with both IPv4 and IPv6 */
  af = IOT_SOCKET_AF_INET;
  ret = iotSocketGetHostByName (host, af, &ip_addr, &ip_len);
#if defined(RTE_Network_IPv6)
  if (ret == IOT_SOCKET_ERROR) {
    af = IOT_SOCKET_AF_INET6;
    ret = iotSocketGetHostByName (host, af, &ip_addr, &ip_len);
  }
#endif
  if (ret < 0) {
    return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
  }

  /* Get port number */
  port_nr = 0;
  sscanf (port, "%hu", &port_nr);
  if (port_nr == 0) {
    return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
  }

  ctx->fd = -1;
  if (proto == MBEDTLS_NET_PROTO_TCP) {
    ctx->fd = iotSocketCreate (af, IOT_SOCKET_TYPE_STREAM, IOT_SOCKET_PROTOCOL_TCP);
  }
  else if (proto == MBEDTLS_NET_PROTO_UDP) {
    ctx->fd = iotSocketCreate (af, IOT_SOCKET_TYPE_DGRAM, IOT_SOCKET_PROTOCOL_UDP);
  }
  if (ctx->fd < 0) {
    return (MBEDTLS_ERR_NET_SOCKET_FAILED);
  }

  if (iotSocketConnect (ctx->fd, ip_addr, ip_len, port_nr) < 0) {
    iotSocketClose (ctx->fd);
    ctx->fd = -1;
    return (MBEDTLS_ERR_NET_CONNECT_FAILED);
  }
  return (0);
}

/*
 * Create a listening socket on bind_ip:port
 */
int mbedtls_net_bind (mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto) {
  uint8_t  ip_addr[IP_LEN];
  uint32_t ip_len = IP_LEN;
  uint16_t port_nr;
  int32_t  af, ret;

  /* Get IPv4 local address */
  af = IOT_SOCKET_AF_INET;
  if (bind_ip == NULL) {
    memset (ip_addr, 0, IP_LEN);
    ip_len = 4;
  }
  else {
    /* Resolve IPv4 absolute address ie. "192.168.0.100" */
    ret = sscanf (bind_ip, "%hhu.%hhu.%hhu.%hhu", &ip_addr[0], &ip_addr[1],
                                                  &ip_addr[2], &ip_addr[3]);
    if (ret != 4) {
      return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
    }
    ip_len = 4;
  }

  /* Get port number */
  port_nr = 0;
  sscanf (port, "%hu", &port_nr);
  if (port_nr == 0) {
    return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
  }

  ctx->fd = -1;
  if (proto == MBEDTLS_NET_PROTO_TCP) {
    ctx->fd = iotSocketCreate (af, IOT_SOCKET_TYPE_STREAM, IOT_SOCKET_PROTOCOL_TCP);
  }
  else if (proto == MBEDTLS_NET_PROTO_UDP) {
    ctx->fd = iotSocketCreate (af, IOT_SOCKET_TYPE_DGRAM, IOT_SOCKET_PROTOCOL_UDP);
  }
  if (ctx->fd < 0) {
    return (MBEDTLS_ERR_NET_SOCKET_FAILED);
  }

  if (iotSocketBind (ctx->fd, ip_addr, ip_len, port_nr) < 0) {
    iotSocketClose (ctx->fd);
    ctx->fd = -1;
    return (MBEDTLS_ERR_NET_BIND_FAILED);
  }

  /* Listen only makes sense for TCP */
  if (proto == MBEDTLS_NET_PROTO_TCP) {
    if (iotSocketListen (ctx->fd, MBEDTLS_NET_LISTEN_BACKLOG) < 0) {
      iotSocketClose (ctx->fd);
      ctx->fd = -1;
      return (MBEDTLS_ERR_NET_LISTEN_FAILED);
    }
  }
  return (0);
}

/*
 * Accept a connection from a remote client
 */
int mbedtls_net_accept (mbedtls_net_context *bind_ctx,
                        mbedtls_net_context *client_ctx,
                        void *client_ip, size_t buf_size, size_t *ip_len) {
  uint32_t iplen = buf_size;
  int32_t  ret;

  ret = iotSocketAccept (bind_ctx->fd, client_ip, &iplen, NULL);
  if (ret < 0) {
    if (ret == IOT_SOCKET_ERROR_WOULDBLOCK) {
      return (MBEDTLS_ERR_SSL_WANT_READ);
    }
    return (MBEDTLS_ERR_NET_ACCEPT_FAILED);
  }
  client_ctx->fd = ret;
  if (ip_len != NULL) {
    *ip_len = iplen;
  }
  return (0);
}

/*
 * Set the socket blocking or non-blocking
 */
int mbedtls_net_set_block (mbedtls_net_context *ctx) {
  uint32_t nbio = 0;

  return (iotSocketSetOpt (ctx->fd, IOT_SOCKET_OPT_NBIO, &nbio, sizeof(nbio)));
}

int mbedtls_net_set_nonblock (mbedtls_net_context *ctx) {
  uint32_t nbio = 1;

  return (iotSocketSetOpt (ctx->fd, IOT_SOCKET_OPT_NBIO, &nbio, sizeof(nbio)));
}

/*
 * Portable usleep helper
 */
void mbedtls_net_usleep (unsigned long usec) {
  osDelay ((usec + 999) / 1000);
}

/*
 * Read at most 'len' characters
 */
int mbedtls_net_recv (void *ctx, unsigned char *buf, size_t len) {
  mbedtls_net_context *ctxt = ctx;
  int32_t ret;

  ret = iotSocketRecv (ctxt->fd, buf, len);
  if (ret < 0) {
    /* Error: return mbedTLS error codes */
    if (ret == IOT_SOCKET_ERROR_SOCKET) {
      return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }
    if (ret == IOT_SOCKET_ERROR_WOULDBLOCK) {
      return (MBEDTLS_ERR_SSL_WANT_READ);
    }
    if (ret == IOT_SOCKET_ERROR_CLOSED) {
      return (MBEDTLS_ERR_NET_CONN_RESET);
    }
    if (ret == IOT_SOCKET_ERROR_TIMEOUT) {
      return (MBEDTLS_ERR_SSL_TIMEOUT);
    }
    return (MBEDTLS_ERR_NET_RECV_FAILED);
  }
  return (ret);
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
int mbedtls_net_recv_timeout (void *ctx, unsigned char *buf, size_t len, uint32_t timeout) {
  mbedtls_net_context *ctxt = ctx;
  uint32_t n = timeout;
  int32_t ret;

  ret = iotSocketSetOpt (ctxt->fd, IOT_SOCKET_OPT_RECV_TIMEOUT, &n, sizeof(n));
  if (ret < 0) {
    /* Error: return mbedTLS error codes */
    if (ret == IOT_SOCKET_ERROR_SOCKET) {
      return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }
    return (MBEDTLS_ERR_NET_RECV_FAILED);
  }

  /* This call will not block */
  return (mbedtls_net_recv (ctxt, buf, len));
}

/*
 * Write at most 'len' characters
 */
int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len ) {
  mbedtls_net_context *ctxt = ctx;
  int32_t ret;

  ret = iotSocketSend (ctxt->fd, buf, len);
  if (ret < 0) {
    /* Error: return mbedTLS error codes */
    if (ret == IOT_SOCKET_ERROR_SOCKET) {
      return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }
    if (ret == IOT_SOCKET_ERROR_WOULDBLOCK) {
      return (MBEDTLS_ERR_SSL_WANT_WRITE);
    }
    if (ret == IOT_SOCKET_ERROR_CLOSED) {
      return (MBEDTLS_ERR_NET_CONN_RESET);
    }
    return (MBEDTLS_ERR_NET_SEND_FAILED);
  }
  return (ret);
}

/*
 * Gracefully close the connection
 */
void mbedtls_net_free (mbedtls_net_context *ctx) {
  if (ctx->fd < 0) {
    return;
  }
  iotSocketClose (ctx->fd);
  ctx->fd = -1;
}

#endif /* MBEDTLS_NET_C */
