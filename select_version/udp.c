//////////////////////////////////////////////////////////////////////////////
//
//      Headers
//
//////////////////////////////////////////////////////////////////////////////
#include "tap.h"

//////////////////////////////////////////////////////////////////////////////
//
//      Global Variables
//
//////////////////////////////////////////////////////////////////////////////
static struct sockaddr_in _sendaddr = {};

//////////////////////////////////////////////////////////////////////////////
//
//      Functions
//
//////////////////////////////////////////////////////////////////////////////
int udp_recv(int udpfd, void* buffer, int bufferSize)
{
    return recv(udpfd, buffer, bufferSize, 0);
}

int udp_send(int udpfd, void* data, int dataLen)
{
    return sendto(udpfd, data, dataLen, 0, (struct sockaddr *)&_sendaddr,
                  sizeof(struct sockaddr_in));
}

int udp_create(char* dstip, int port)
{
    int udpfd = -1;

    CHECK_IF(NULL==dstip, goto _ERROR, "dstip is null");

    udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    CHECK_IF(0>udpfd, goto _ERROR, "udp socket create failed");

    struct sockaddr_in recvaddr = {};

    recvaddr.sin_family      = AF_INET;
    recvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    recvaddr.sin_port        = htons(port);

    int flag = 1;
    int ret  = setsockopt(udpfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
    CHECK_IF(0>ret, goto _ERROR, "udp socket setsockopt failed");

    ret = bind(udpfd, (struct sockaddr *)&recvaddr, sizeof(struct sockaddr_in));
    CHECK_IF(0>ret, goto _ERROR, "udp socket bind failed");

    _sendaddr.sin_family = AF_INET;
    ret = inet_pton(AF_INET, dstip, &(_sendaddr.sin_addr.s_addr));
    CHECK_IF(0>ret, goto _ERROR, "dstip pton failed");
    _sendaddr.sin_port = htons(port);

    return udpfd;

_ERROR:
    if (udpfd > 0)
    {
        close(udpfd);
    }
    return -1;
}
