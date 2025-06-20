This is an SSL client example running on the MDK-Middleware Network stack.

The code for "ssl_client1.c" is available on:
https://github.com/Mbed-TLS/mbedtls/blob/development/programs/ssl/ssl_client1.c

The only change to the original code is the server name used to establish a connection,
the rest of the code has not been changed.

More information about SSL client example is available here:
https://tls.mbed.org/kb/how-to/mbedtls-tutorial

This example connects to the "ssl_server" application and reads a default page.
An Internet connection is not required. The status messages are printed out on the terminal.
