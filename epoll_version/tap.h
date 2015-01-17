#ifndef _TAP_H_
#define _TAP_H_

//////////////////////////////////////////////////////////////////////////////
//
//      Headers
//
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <sys/select.h>

#include <signal.h>
#include <net/if.h>         // struct ifreq
#include <sys/types.h>      // open
#include <sys/stat.h>       // open
#include <fcntl.h>          // open
#include <arpa/inet.h>      // inet
#include <sys/ioctl.h>      // ioctl
#include <linux/if_tun.h>   // tun/tap
#include <errno.h>
#include <unistd.h>         // close
#include <string.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <sys/epoll.h>

//////////////////////////////////////////////////////////////////////////////
//
//      Macros
//
//////////////////////////////////////////////////////////////////////////////
#define dprint(a, b...) printf("%s(): "a"\n", __func__, ##b)

#define derror(a, b...) dprint("[ERROR] "a, ##b)

#define CHECK_IF(assertion, error_action, ...) \
{\
    if (assertion) \
    { \
        derror(__VA_ARGS__); \
        {error_action;} \
    }\
}

#define FN_APPLY_ALL(type, fn, ...) \
{\
    void* _stopPoint = (int[]){0};\
    void** _listForApplyAll = (type[]){__VA_ARGS__, _stopPoint};\
    int i;\
    for (i=0; _listForApplyAll[i] != _stopPoint; i++)\
    {\
        fn(_listForApplyAll[i]);\
    }\
}

#define FREE_ALL(...) FN_APPLY_ALL(void*, my_free, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////
//
//      Define Values
//
//////////////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE  2048
#define TUN_FILEPATH  "/dev/net/tun"
#define TAP_IF_DEFAULT_NAME "tap01"
#define TUNNEL_DEFAULT_PORT 50000
/* 60 bytes is max size of ip header */
#define TUNNEL_HDR_SIZE (sizeof(struct ethhdr)+60+sizeof(struct udphdr))

//////////////////////////////////////////////////////////////////////////////
//
//      Inline Functions
//
//////////////////////////////////////////////////////////////////////////////
static inline my_free(void* ptr)
{
    if (ptr) { free(ptr); }
}

//////////////////////////////////////////////////////////////////////////////
//
//      Function Declarations
//
//////////////////////////////////////////////////////////////////////////////
int tap_setip(char* ifname, char* ipaddr);
int tap_setmask(char* ifname, char* netmask);
int tap_enable(char* ifname);
int tap_setMtu(char* ifname);
int tap_alloc(char* ifname, int flags);

int udp_recv(int udpfd, void* buffer, int bufferSize);
int udp_send(int udpfd, void* data, int dataLen);
int udp_create(char* dstip, int port);

#endif
//_TAP_H_
