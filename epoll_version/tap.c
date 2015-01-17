//////////////////////////////////////////////////////////////////////////////
//
//      Headers
//
//////////////////////////////////////////////////////////////////////////////
#include "tap.h"

//////////////////////////////////////////////////////////////////////////////
//
//      Functions
//
//////////////////////////////////////////////////////////////////////////////
int tap_setip(char* ifname, char* ipaddr)
{
    CHECK_IF(NULL==ifname, return -1, "ifname is null");
    CHECK_IF(NULL==ipaddr, return -1, "ipaddr is null");

    int fd = socket( PF_INET, SOCK_DGRAM, IPPROTO_IP);
    CHECK_IF(0>fd, return fd, "open socket failed");

    struct ifreq ifr = {.ifr_addr.sa_family = AF_INET};
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    int err = inet_pton(AF_INET, ipaddr, ifr.ifr_addr.sa_data + 2);
    CHECK_IF(1!=err, goto _ERROR, "error ip address = %s", ipaddr);

    err = ioctl(fd, SIOCSIFADDR, &ifr);
    CHECK_IF(0>err, goto _ERROR, "ioctl SIOCSIFADDR failed");

    close(fd);
    return 0;

_ERROR:
    close(fd);
    return err;
}

int tap_setmask(char* ifname, char* netmask)
{
    CHECK_IF(NULL==ifname, return -1, "ifname is null");
    CHECK_IF(NULL==netmask, return -1, "netmask is null");

    int fd = socket( PF_INET, SOCK_DGRAM, IPPROTO_IP );
    CHECK_IF(0>fd, return fd, "open socket failed");

    struct ifreq ifr = {.ifr_addr.sa_family = AF_INET};
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    int err = inet_pton(AF_INET, netmask, ifr.ifr_addr.sa_data + 2);
    CHECK_IF(1!=err, goto _ERROR, "error netmask = %s", netmask);

    err = ioctl(fd, SIOCSIFNETMASK, &ifr);
    CHECK_IF(0>err, goto _ERROR, "ioctl SIOCSIFNETMASK failed");

    close(fd);
    return 0;

_ERROR:
    close(fd);
    return err;
}

int tap_enable(char* ifname)
{
    CHECK_IF(NULL==ifname, return -1, "ifname is null");

    int fd = socket( PF_INET, SOCK_DGRAM, IPPROTO_IP );
    CHECK_IF(0>fd, return fd, "open socket failed");

    struct ifreq ifr = {.ifr_addr.sa_family = AF_INET};
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    int err = ioctl(fd, SIOCGIFFLAGS, &ifr);
    CHECK_IF(0>err, goto _ERROR, "ioctl SIOCGIFFLAGS failed");

    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);

    err = ioctl(fd, SIOCSIFFLAGS, &ifr);
    CHECK_IF(0>err, goto _ERROR, "ioctl SIOCSIFFLAGS failed");

    close(fd);
    return 0;

_ERROR:
    close(fd);
    return err;
}

int tap_setMtu(char* ifname)
{
    CHECK_IF(NULL==ifname, return -1, "ifname is null");

    int fd = socket( PF_INET, SOCK_DGRAM, IPPROTO_IP );
    CHECK_IF(0>fd, return fd, "open socket failed");

    struct ifreq ifr = {.ifr_addr.sa_family = AF_INET};
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    int err = ioctl(fd, SIOCGIFMTU, &ifr);
    CHECK_IF(0>err, goto _ERROR, "ioctl SIOCGIFFLAGS failed");
    CHECK_IF(TUNNEL_HDR_SIZE >= ifr.ifr_mtu, goto _ERROR,
             "new mtu size = %d is too small", ifr.ifr_mtu);

    ifr.ifr_mtu -= TUNNEL_HDR_SIZE;
    err = ioctl(fd, SIOCSIFMTU, &ifr);
    CHECK_IF(0>err, goto _ERROR, "ioctl SIOCSIFFLAGS failed");

    close(fd);
    return 0;

_ERROR:
    close(fd);
    return err;
}

int tap_alloc(char* ifname, int flags)
{
    CHECK_IF(NULL==ifname, return -1, "ifname is null");

    int fd = open(TUN_FILEPATH, O_RDWR);
    CHECK_IF(0>fd, return fd, "open %s failed", TUN_FILEPATH);

    struct ifreq ifr = {.ifr_flags = flags};
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    int err = ioctl(fd, TUNSETIFF, (void*)&ifr);
    CHECK_IF(0>err, goto _ERROR, "ioctl TUNSETIFF failed");

    return fd;

_ERROR:
    close(fd);
    return err;
}
