#include "udp.h"
#include <network.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* A ripoff of logger.c */

static int udp_socket = -1;
static volatile int udp_lock = 0;


void udp_init(std::string_view ipString, unsigned short ipport)
{
    udp_socket = net_socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (udp_socket < 0) {
        return;
    }

    struct sockaddr_in connect_addr;
    memset(&connect_addr, 0, sizeof(connect_addr));
    connect_addr.sin_family = AF_INET;
    connect_addr.sin_port = ipport;
    inet_aton(ipString.data(), &connect_addr.sin_addr);

    if(net_connect(udp_socket, (struct sockaddr*)&connect_addr, sizeof(connect_addr)) < 0)
    {
        net_close(udp_socket);
        udp_socket = -1;
    }
}

void udp_deinit(void)
{
    if(udp_socket >= 0)
    {
        net_close(udp_socket);
        udp_socket = -1;
    }
}

void udp_print(const char *str)
{
    // socket is always 0 initially as it is in the BSS
    if(udp_socket < 0) {
        return;
    }

    while(udp_lock) {
        usleep(1000);
    }
    udp_lock = 1;

    int len = strlen(str);
    while (len > 0) {
        const auto block = std::min(len, 1400); // take max 1400 bytes per UDP packet
        const auto ret = net_send(udp_socket, str, block, 0);
        if(ret < 0) {
            break;
        }

        len -= ret;
        str += ret;
    }

    udp_lock = 0;
}
