/*
 *  TCP/IP or UDP/IP networking functions for MDK-Pro Network Dual Stack
 *
 *  Copyright (C) 2006-2022, Arm Limited, All Rights Reserved
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
 */

#include "mbedtls/build_info.h"

#if defined(MBEDTLS_NET_C)

#if (defined(_WIN32) || defined(_WIN32_WCE) || defined(EFIX64) || defined(EFI32))
 #error "Wrong environment, MDK-Pro required"
#endif

#include <stdio.h>
#include <string.h>
#include "mbedtls/net_sockets.h"
#include "rl_net.h"
#include "RTE_Components.h"
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

#define SOCK_ADDR(addr)      ((SOCKADDR *)    addr)
#define SOCK_ADDR4(addr)     ((SOCKADDR_IN *) addr)
#define SOCK_ADDR6(addr)     ((SOCKADDR_IN6 *)addr)

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
 * Initiate a TCP connection with host:port and the given protocol
 */
int mbedtls_net_connect (mbedtls_net_context *ctx, const char *host, const char *port, int proto) {
  SOCKADDR_STORAGE host_addr;
  NET_ADDR  addr;
  netStatus stat;
  int32_t ret, addrlen;

  if ((ret = net_prepare ()) != 0) {
    return (ret);
  }

  /* Do name resolution with both IPv4 and IPv6 */
  stat = netDNSc_GetHostByNameX (host, NET_ADDR_IP4, &addr);
#if defined(RTE_Network_IPv6)
  if (stat == netDnsResolverError) {
    /* Failed for IPv4, retry for IPv6 */
    stat = netDNSc_GetHostByNameX (host, NET_ADDR_IP6, &addr);
  }
#endif
  if (stat != netOK) {
    return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
  }

  /* Get port number */
  addr.port = 0;
  sscanf (port, "%hu", &addr.port);
  if (addr.port == 0) {
    return (MBEDTLS_ERR_NET_UNKNOWN_HOST);
  }

  if (addr.addr_type == NET_ADDR_IP4) {
    SOCK_ADDR4(&host_addr)->sin_family = AF_INET;
    SOCK_ADDR4(&host_addr)->sin_port   = htons (addr.port);
    memcpy (&SOCK_ADDR4(&host_addr)->sin_addr, addr.addr, NET_ADDR_IP4_LEN);
    addrlen = sizeof(SOCKADDR_IN);
  }
#if defined(RTE_Network_IPv6)
  else {
    SOCK_ADDR6(&host_addr)->sin6_family = AF_INET6;
    SOCK_ADDR6(&host_addr)->sin6_port   = htons (addr.port);
    memcpy (&SOCK_ADDR6(&host_addr)->sin6_addr, addr.addr, NET_ADDR_IP6_LEN);
    addrlen = sizeof(SOCKADDR_IN6);
  }
#endif
  ctx->fd = socket (SOCK_ADDR(&host_addr)->sa_family,
                    (proto == MBEDTLS_NET_PROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM, 0);
  if (ctx->fd < 0) {
    return (MBEDTLS_ERR_NET_SOCKET_FAILED);
  }

  if (connect (ctx->fd, SOCK_ADDR(&host_addr), addrlen) < 0) {
    closesocket (ctx->fd);
    ctx->fd = -1;
    return (MBEDTLS_ERR_NET_CONNECT_FAILED);
  }
  return (0);
}

/*
 * Create a listening socket on bind_ip:port
 */
int mbedtls_net_bind (mbedtls_net_context *ctx, const char *bind_ip, const char *port, int proto) {
  SOCKADDR_STORAGE host_addr;
  uint16_t port_nr;
  int32_t ret, addrlen;

  if ((ret = net_prepare ()) != 0) {
    return (ret);
  }

  /* Get IPv4 or IPv6 address */
  memset (&host_addr, 0, sizeof(host_addr));
  if (bind_ip == NULL) {
    SOCK_ADDR4(&host_addr)->sin_family = AF_INET;
    addrlen = sizeof(SOCKADDR_IN);
  }
  else if (inet_pton (AF_INET, bind_ip, &SOCK_ADDR4(&host_addr)->sin_addr)) {
    SOCK_ADDR4(&host_addr)->sin_family = AF_INET;
    addrlen = sizeof(SOCKADDR_IN);
  }
#if defined(RTE_Network_IPv6)
  else if (inet_pton (AF_INET6, bind_ip, &SOCK_ADDR6(&host_addr)->sin6_addr)) {
    SOCK_ADDR6(&host_addr)->sin6_family = AF_INET6;
    addrlen = sizeof(SOCKADDR_IN6);
  }
#endif
  else {
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

  if (bind (ctx->fd, SOCK_ADDR(&host_addr), addrlen) < 0) {
    closesocket (ctx->fd);
    return (MBEDTLS_ERR_NET_BIND_FAILED);
  }

  /* Listen only makes sense for TCP */
  if (proto == MBEDTLS_NET_PROTO_TCP) {
    if (listen (ctx->fd, MBEDTLS_NET_LISTEN_BACKLOG) < 0) {
      closesocket (ctx->fd);
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
  SOCKADDR_STORAGE client_addr;
  int32_t type, type_len = sizeof (type);
  int32_t ret, n = sizeof(client_addr);

  /* Is this a TCP or UDP socket? */
  if (getsockopt (bind_ctx->fd, SOL_SOCKET, SO_TYPE, (char *)&type, &type_len) < 0) {
    return (MBEDTLS_ERR_NET_ACCEPT_FAILED);
  }
  if (type == SOCK_STREAM) {
    /* TCP: actual accept() */
    ret = accept (bind_ctx->fd, SOCK_ADDR(&client_addr), &n);
    client_ctx->fd = (ret < 0) ? -1 : ret;
  }
  else {
    /* UDP: wait for a message, but keep it in the queue */
    char buf[1] = { 0 };
    ret = recvfrom (bind_ctx->fd, buf, sizeof (buf), MSG_PEEK,
                    SOCK_ADDR(&client_addr), &n );
  }

  if (ret < 0) {
    if (ret == BSD_EWOULDBLOCK) {
      return (MBEDTLS_ERR_SSL_WANT_READ);
    }
    return (MBEDTLS_ERR_NET_ACCEPT_FAILED);
  }

  /* UDP: currently only one connection is accepted */
  /*      (multiplexing is not supported)           */
  if (type != SOCK_STREAM) {
    /* Enable address filtering */
    if (connect (bind_ctx->fd, SOCK_ADDR(&client_addr), n) < 0) {
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

  return ((ioctlsocket (ctx->fd, FIONBIO, &n) < 0) ? -1 : 0);
}

int mbedtls_net_set_nonblock (mbedtls_net_context *ctx) {
  unsigned long n = 1;

  return ((ioctlsocket (ctx->fd, FIONBIO, &n) < 0) ? -1 : 0);
}

/*
 * Check if data is available on the socket
 */

int mbedtls_net_poll (mbedtls_net_context *ctx, uint32_t rw, uint32_t timeout) {
  struct timeval tv;
  fd_set read_fds;
  fd_set write_fds;
  int32_t ret;

  if (ctx->fd < 0) {
    return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
  }

  FD_ZERO (&read_fds);
  if (rw & MBEDTLS_NET_POLL_READ) {
    rw &= ~MBEDTLS_NET_POLL_READ;
    FD_SET (ctx->fd, &read_fds);
  }

  FD_ZERO (&write_fds);
  if ((rw & MBEDTLS_NET_POLL_WRITE) && (timeout == 0)) {
    rw &= ~MBEDTLS_NET_POLL_WRITE;
    FD_SET (ctx->fd, &write_fds);
  }

  if (rw != 0) {
    return (MBEDTLS_ERR_NET_BAD_INPUT_DATA);
  }
  tv.tv_sec  = timeout / 1000;
  tv.tv_usec = (timeout % 1000) * 1000;

  ret = select (ctx->fd + 1, &read_fds, &write_fds, NULL,
                (timeout == (uint32_t)-1) ? NULL : &tv);
  if (ret < 0) {
    return (MBEDTLS_ERR_NET_POLL_FAILED);
  }

  ret = 0;
  if (FD_ISSET (ctx->fd, &read_fds)) {
    ret |= MBEDTLS_NET_POLL_READ;
  }
  if (FD_ISSET (ctx->fd, &write_fds)) {
    ret |= MBEDTLS_NET_POLL_WRITE;
  }
  return (ret);
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
  mbedtls_net_context *ctx_io = ctx;
  int32_t ret;

  ret = recv (ctx_io->fd, (char *)buf, len, 0);
  if (ret < 0) {
    /* Error: return mbedTLS error codes */
    if (ret == BSD_ESOCK) {
      return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }
    if ((ret == BSD_EWOULDBLOCK) || (ret == BSD_ETIMEDOUT)) {
      return (MBEDTLS_ERR_SSL_WANT_READ);
    }
    if (ret == BSD_ECONNRESET) {
      return (MBEDTLS_ERR_NET_CONN_RESET);
    }
    return (MBEDTLS_ERR_NET_RECV_FAILED);
  }
  return (ret);
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
int mbedtls_net_recv_timeout (void *ctx, unsigned char *buf, size_t len, uint32_t timeout) {
  mbedtls_net_context *ctx_io = ctx;
  struct timeval tv;
  fd_set read_fds;
  int32_t ret;

  if (ctx_io->fd < 0) {
    return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
  }
  FD_ZERO (&read_fds);
  FD_SET (ctx_io->fd, &read_fds);

  tv.tv_sec  = timeout / 1000;
  tv.tv_usec = (timeout % 1000) * 1000;

  ret = select (ctx_io->fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv);

  /* Zero fds ready means we timed out */
  if (ret == 0) {
    return (MBEDTLS_ERR_SSL_TIMEOUT);
  }
  if (ret < 0) {
    return (MBEDTLS_ERR_NET_RECV_FAILED);
  }

  /* This call will not block */
  return (mbedtls_net_recv (ctx, buf, len));
}

/*
 * Write at most 'len' characters
 */
int mbedtls_net_send (void *ctx, const unsigned char *buf, size_t len) {
  mbedtls_net_context *ctx_io = ctx;
  int32_t ret;

  ret = send (ctx_io->fd, (const char *)buf, len, 0);
  if (ret < 0) {
    /* Error: return mbedTLS error codes */
    if (ret == BSD_ESOCK) {
      return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }
    if ((ret == BSD_EWOULDBLOCK) || (ret == BSD_ETIMEDOUT)) {
      return (MBEDTLS_ERR_SSL_WANT_WRITE);
    }
    if (ret == BSD_ECONNRESET) {
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
  closesocket (ctx->fd);
  ctx->fd = -1;
}

#endif /* MBEDTLS_NET_C */
