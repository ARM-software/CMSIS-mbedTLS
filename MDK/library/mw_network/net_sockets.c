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

#if (defined(_WIN32) || defined(_WIN32_WCE) || defined(EFIX64) || defined(EFI32))
 #error "Wrong environment, MDK-Pro required"
#endif

#include <stdio.h>
#include <string.h>
#include "RTE_Components.h"
#include "mbedtls/net_sockets.h"
#include "rl_net.h"
#if defined(RTE_CMSIS_RTOS)
 #include "cmsis_os.h"
#elif defined(RTE_CMSIS_RTOS2)
 #include "cmsis_os2.h"
#else
 #error "::CMSIS:RTOS selection invalid"
#endif

#if !defined(RTE_Network_IPv4) && !defined(RTE_Network_IPv6)
 #error "MDK-Pro Network component required"
#endif

#if !defined(RTE_Network_Socket_BSD)
 #error "::Network:Socket:BSD: BSD Sockets not enabled"
#endif

#if !defined(RTE_Network_DNS_Client)
 #error "::Network:Service:DNS Client: DNS Client not enabled"
#endif

#undef MBEDTLS_NET_LISTEN_BACKLOG
#define MBEDTLS_NET_LISTEN_BACKLOG      3

#if defined(RTE_CMSIS_RTOS)
 static osThreadId   dns_thread;
#else
 static osThreadId_t dns_thread;
#endif
static SOCKADDR     *dns_addr;

#define SOCK_ADDR(addr)      ((SOCKADDR *)    addr)
#define SOCK_ADDR4(addr)     ((SOCKADDR_IN *) addr)
#define SOCK_ADDR6(addr)     ((SOCKADDR_IN6 *)addr)
#define SOCK(ctx)            (((mbedtls_net_context *)ctx)->fd >> 1)

/*
 * Prepare for using the sockets interface
 */
static int net_prepare (void) {
  /* Verify that the Network Component is not already running */
  if (netSYS_GetHostName () == NULL) {
    if (netInitialize () != netOK) {
      return (MBEDTLS_ERR_NET_SOCKET_FAILED);
    }
    /* Small delay for Network to setup */
    osDelay (500);
  }
  return (0);
}

/*
 * Initialize a context
 */
void mbedtls_net_init (mbedtls_net_context *ctx) {
  ctx->fd = -1;
}

/*
 * DNS resolver callback
 */
static void dns_cbfunc (netDNSc_Event event, const NET_ADDR *addr) {
  if (dns_addr && event == netDNSc_EventSuccess) {
    if (addr->addr_type == NET_ADDR_IP4) {
      SOCK_ADDR4(dns_addr)->sin_family = AF_INET;
      memcpy (&SOCK_ADDR4(dns_addr)->sin_addr, addr->addr, NET_ADDR_IP4_LEN);
    }
#if defined(RTE_Network_IPv6)
    if (addr->addr_type == NET_ADDR_IP6) {
      SOCK_ADDR6(dns_addr)->sin6_family = AF_INET6;
      memcpy (&SOCK_ADDR6(dns_addr)->sin6_addr, addr->addr, NET_ADDR_IP6_LEN);
    }
#endif
    dns_addr = NULL;
  }
  if (dns_thread) {
#if defined(RTE_CMSIS_RTOS)
    osSignalSet (dns_thread, 0x01);
#else
    osThreadFlagsSet (dns_thread, 0x01);
#endif
  }
}

/*
 * Initiate a TCP connection with host:port and the given protocol
 */
int mbedtls_net_connect (mbedtls_net_context *ctx, const char *host, const char *port, int proto) {
#if defined(RTE_Network_IPv6)
  SOCKADDR_IN6 host_addr;
#else
  SOCKADDR_IN  host_addr;
#endif
  netStatus stat;
  int16_t addr_type;
  uint16_t port_nr;
  int ret;

  if ((ret = net_prepare ()) != 0) {
    return (ret);
  }

  /* Do name resolution with both IPv4 and IPv6 */
  SOCK_ADDR(&host_addr)->sa_family = AF_UNSPEC;
  dns_addr  = SOCK_ADDR(&host_addr);
  addr_type = NET_ADDR_IP4;
  while (1) {
    stat = netDNSc_GetHostByName (host, addr_type, dns_cbfunc);
    if (stat == netBusy) {
      osDelay (100);
      continue;
    }
    if (stat == netOK) {
      dns_thread = osThreadGetId ();
#if defined(RTE_CMSIS_RTOS)
      osSignalWait (0x01, osWaitForever);
#else
      osThreadFlagsWait (0x01, osFlagsWaitAny, osWaitForever);
#endif
      dns_thread = NULL;
      if (SOCK_ADDR(&host_addr)->sa_family != AF_UNSPEC) {
        break;
      }
    }
#if defined(RTE_Network_IPv6)
    if (addr_type == NET_ADDR_IP4) {
      /* Failed with IPv4, retry with IPv6 */
      addr_type = NET_ADDR_IP6;
      continue;
    }
#endif
    return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
  }
  /* Get port number */
  port_nr = 0;
  sscanf (port, "%hu", &port_nr);
  if (port_nr == 0) {
    return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
  }
  if (SOCK_ADDR(&host_addr)->sa_family == AF_INET) {
    SOCK_ADDR4(&host_addr)->sin_port = htons (port_nr);
  }
#if defined(RTE_Network_IPv6)
  else {
    SOCK_ADDR6(&host_addr)->sin6_port = htons (port_nr);
  }
#endif
  ctx->fd = socket (SOCK_ADDR(&host_addr)->sa_family,
                    (proto == MBEDTLS_NET_PROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM, 0);
  if (ctx->fd < 0) {
    return (MBEDTLS_ERR_NET_SOCKET_FAILED);
  }

  /* Encode SOCK type into fd (bit 0) */
  ctx->fd <<= 1;
  if (proto == MBEDTLS_NET_PROTO_TCP) ctx->fd |= 1;

  if (connect (SOCK(ctx), SOCK_ADDR(&host_addr), sizeof(host_addr)) < 0) {
    closesocket (SOCK(ctx));
    ctx->fd = -1;
    return (MBEDTLS_ERR_NET_CONNECT_FAILED);
  }
  return (0);
}

/*
 * Create a listening socket on bind_ip:port
 */
int mbedtls_net_bind (mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto) {
#if defined(RTE_Network_IPv6)
  SOCKADDR_IN6 host_addr;
#else
  SOCKADDR_IN  host_addr;
#endif
  uint16_t port_nr;
  int ret;

  if ((ret = net_prepare ()) != 0) {
    return (ret);
  }

  /* Get IPv4 or IPv6 address */
  memset (&host_addr, 0, sizeof(host_addr));
  if (bind_ip == NULL) {
    SOCK_ADDR4(&host_addr)->sin_family = AF_INET;
  }
  else if (netIP_aton (bind_ip, NET_ADDR_IP4, (uint8_t *)&SOCK_ADDR4(&host_addr)->sin_addr)) {
    SOCK_ADDR4(&host_addr)->sin_family = AF_INET;
  }
#if defined(RTE_Network_IPv6)
  else if (netIP_aton (bind_ip, NET_ADDR_IP6, (uint8_t *)&SOCK_ADDR6(&host_addr)->sin6_addr)) {
    SOCK_ADDR6(&host_addr)->sin6_family = AF_INET6;
  }
#endif

  /* Get port number */
  port_nr = 0;
  sscanf (port, "%hu", &port_nr);
  if (port_nr == 0) {
    return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
  }
  if (SOCK_ADDR(&host_addr)->sa_family == AF_INET) {
    SOCK_ADDR4(&host_addr)->sin_port = htons (port_nr);
  }
#if defined(RTE_Network_IPv6)
  else {
    SOCK_ADDR6(&host_addr)->sin6_port = htons (port_nr);
  }
#endif

  ctx->fd = socket (SOCK_ADDR(&host_addr)->sa_family,
                    (proto == MBEDTLS_NET_PROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM, 0);
  if (ctx->fd < 0) {
    return (MBEDTLS_ERR_NET_SOCKET_FAILED);
  }

  /* Encode SOCK type into fd (bit 0) */
  ctx->fd <<= 1;
  if (proto == MBEDTLS_NET_PROTO_TCP) ctx->fd |= 1;

  if (bind (SOCK(ctx), SOCK_ADDR(&host_addr), sizeof (host_addr)) != BSD_SUCCESS) {
    closesocket (SOCK(ctx));
    return (MBEDTLS_ERR_NET_BIND_FAILED);
  }

  /* Listen only makes sense for TCP */
  if (proto == MBEDTLS_NET_PROTO_TCP) {
    if (listen (SOCK(ctx), MBEDTLS_NET_LISTEN_BACKLOG) != BSD_SUCCESS) {
      closesocket (SOCK(ctx));
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
#if defined(RTE_Network_IPv6)
  SOCKADDR_IN6 client_addr;
#else
  SOCKADDR_IN  client_addr;
#endif
  int ret, n = sizeof(client_addr);

  /* Is this a TCP or UDP socket? */
  if (bind_ctx->fd & 1) {
    /* TCP: actual accept() */
    ret = accept (SOCK(bind_ctx), SOCK_ADDR(&client_addr), &n);
    /* Encode SOCK type into fd (bit 0) */
    client_ctx->fd = (ret < 0) ? -1 : (ret << 1) | 1;
  }
  else {
    /* UDP: wait for a message, but keep it in the queue */
    char buf[1] = { 0 };
    ret = recvfrom (SOCK(bind_ctx), buf, sizeof (buf), MSG_PEEK,
                    SOCK_ADDR(&client_addr), &n );
  }

  if (ret < 0) {
    if (ret == BSD_ERROR_WOULDBLOCK) {
      return (MBEDTLS_ERR_SSL_WANT_READ);
    }
    return (MBEDTLS_ERR_NET_ACCEPT_FAILED);
  }

  /* UDP: currently only one connection is accepted */
  /*      (multiplexing is not supported)           */
  if (!(bind_ctx->fd & 1)) {
    /* Enable address filtering */
    if (connect (SOCK(bind_ctx), SOCK_ADDR(&client_addr), n) != BSD_SUCCESS) {
      return (MBEDTLS_ERR_NET_ACCEPT_FAILED);
    }
    client_ctx->fd = bind_ctx->fd;
    bind_ctx->fd   = -1;
  }

  /* Copy client address */
  if (client_ip != NULL) {
    if (SOCK_ADDR(&client_addr)->sa_family == AF_INET) {
      *ip_len = sizeof (SOCK_ADDR4(&client_addr)->sin_addr.s_addr);
      if (buf_size < *ip_len) {
        return (MBEDTLS_ERR_NET_BUFFER_TOO_SMALL);
      }
      memcpy (client_ip, &SOCK_ADDR4(&client_addr)->sin_addr.s_addr, *ip_len);
    }
#if defined(RTE_Network_IPv6)
    else {
      *ip_len = sizeof (SOCK_ADDR6(&client_addr)->sin6_addr.s6_addr);
      if (buf_size < *ip_len) {
        return (MBEDTLS_ERR_NET_BUFFER_TOO_SMALL);
      }
      memcpy (client_ip, &SOCK_ADDR6(&client_addr)->sin6_addr.s6_addr, *ip_len);
    }
#endif
  }
  return (0);
}

/*
 * Set the socket blocking or non-blocking
 */
int mbedtls_net_set_block (mbedtls_net_context *ctx) {
  unsigned long n = 0;

  return ((ioctlsocket (SOCK(ctx), FIONBIO, &n) == BSD_SUCCESS) ? 0 : -1);
}

int mbedtls_net_set_nonblock (mbedtls_net_context *ctx) {
  unsigned long n = 1;

  return ((ioctlsocket (SOCK(ctx), FIONBIO, &n) == BSD_SUCCESS) ? 0 : -1);
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
  int ret;

  ret = recv (SOCK(ctx), (char *)buf, len, 0);
  if (ret < 0) {
    /* Error: return mbedTLS error codes */
    if (ret == BSD_ERROR_SOCKET) {
      return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }
    if (ret == BSD_ERROR_WOULDBLOCK) {
      return (MBEDTLS_ERR_SSL_WANT_READ);
    }
    if (ret == BSD_ERROR_CLOSED) {
      return (MBEDTLS_ERR_NET_CONN_RESET);
    }
    if (ret == BSD_ERROR_TIMEOUT) {
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
  uint32_t n = timeout;
  int ret;

  ret = setsockopt (SOCK(ctx), SOL_SOCKET, SO_RCVTIMEO, (const char *)&n, sizeof(n));
  if (ret < 0) {
    /* Error: return mbedTLS error codes */
    if (ret == BSD_ERROR_SOCKET) {
      return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }
    return (MBEDTLS_ERR_NET_RECV_FAILED);
  }

  /* This call will not block */
  return (mbedtls_net_recv (ctx, buf, len));
}

/*
 * Write at most 'len' characters
 */
int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len ) {
  int ret;

  ret = send (SOCK(ctx), (const char *)buf, len, 0);
  if (ret < 0) {
    /* Error: return mbedTLS error codes */
    if (ret == BSD_ERROR_SOCKET) {
      return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }
    if (ret == BSD_ERROR_WOULDBLOCK) {
      return (MBEDTLS_ERR_SSL_WANT_WRITE);
    }
    if (ret == BSD_ERROR_CLOSED) {
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
  closesocket (SOCK(ctx));
  ctx->fd = -1;
}

#endif /* MBEDTLS_NET_C */
