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
static int _running = 1;

//////////////////////////////////////////////////////////////////////////////
//
//      Static Functions
//
//////////////////////////////////////////////////////////////////////////////
static void _sigIntHandler(int signo)
{
    if (signo == SIGINT)
    {
        _running = 0;
    }
}

static void _pringUsage(const char* programName, FILE* stream, int exitCode)
{
    CHECK_IF(NULL==programName, goto _END, "programName is null");
    CHECK_IF(NULL==stream, goto _END, "stream is null");

    fprintf(stream, "Usage %s options ...\n", programName);
    fprintf(stream,
            "\n"
            "\t--ip=[x.x.x.x]    : my tap interface ip address\n"
            "\t--mask=[x.x.x.x]  : my tap interface netmask\n"
            "\t--dst=[x.x.x.x]   : tunnel desination real ip address\n"
            "\t--port=[0~65535]  : (optional) tunnel port, default is 50000\n"
            "\t--name=[string]   : (optional) my tap interface name, default is tap01\n"
            "\t--help            : display usage information.\n"
            "\n"
            );

_END:
    exit(exitCode);
}

//////////////////////////////////////////////////////////////////////////////
//
//      Main Function
//
//////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    const char* programName = argv[0];
    if (2 > argc)
    {
        _pringUsage(programName, stderr, -1);
    }

    /*
    *   get necessary parameters from input
    */
    const struct option longOpt[] = {
        {"ip"  , required_argument, NULL, 1},
        {"mask", required_argument, NULL, 2},
        {"dst" , required_argument, NULL, 3},
        {"port", optional_argument, NULL, 4},
        {"name", optional_argument, NULL, 5},
        {"help", no_argument      , NULL, 6},
        {0     , 0                , 0   , 0}
    };

    int option    = 0;
    char* ipaddr  = NULL;
    char* netmask = NULL;
    char* dstip   = NULL;
    int port      = -1;
    char* ifname  = NULL;

    while (-1 != option)
    {
        option = getopt_long_only(argc, argv, "", longOpt, NULL);
        switch (option)
        {
            case 1:
                asprintf(&ipaddr, "%s", optarg);
                break;

            case 2:
                asprintf(&netmask, "%s", optarg);
                break;

            case 3:
                asprintf(&dstip, "%s", optarg);
                break;

            case 4:
                port = atoi(optarg);
                break;

            case 5:
                asprintf(&ifname, "%s", optarg);
                break;

            case 6:
                _pringUsage(programName, stdout, 0);
                break;

            case -1:
                // end
                break;

            default:
                _pringUsage(programName, stderr, -1);
                break;
        }
    }

    /*
    *   check if gotten parameters are valid
    */
    if (port == -1)
    {
        port = TUNNEL_DEFAULT_PORT;
    }

    if (NULL == ifname)
    {
        asprintf(&ifname, "%s", TAP_IF_DEFAULT_NAME);
    }

    /*
    *   configure TAP
    */
    int udpfd = -1;
    int tapfd = -1;

    tapfd = tap_alloc(ifname, IFF_TAP | IFF_NO_PI);
    CHECK_IF(0>tapfd, goto _END, "tap alloc failed");

    int ret = tap_setip(ifname, ipaddr);
    CHECK_IF(0>ret, goto _END, "tap set ip failed");

    ret = tap_setmask(ifname, netmask);
    CHECK_IF(0>ret, goto _END, "tap set netmask failed");

    ret = tap_setMtu(ifname);
    CHECK_IF(0>ret, goto _END, "tap set mtu failed");

    ret = tap_enable(ifname);
    CHECK_IF(0>ret, goto _END, "tap enable failed");

    signal(SIGINT, _sigIntHandler);

    /*
    *   configure UDP
    */
    udpfd = udp_create(dstip, port);
    CHECK_IF(0>udpfd, goto _END, "udp create failed");

    fd_set readset;
    int selectRet = 0;
    struct timeval timeout;

    unsigned char buffer[BUFFER_SIZE] = {};
    int recvLen = 0;
    int sendLen = 0;

    /*
    *   main process loop
    */
    while (_running)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&readset);
        FD_SET(tapfd, &readset);
        FD_SET(udpfd, &readset);

        selectRet = select(FD_SETSIZE, &readset, NULL, NULL, &timeout);
        if (0 > selectRet)
        {
            break;
        }
        else if (0 == selectRet)
        {
            continue; /* timeout */
        }
        else
        {
            if (FD_ISSET(tapfd, &readset))
            {
                /*
                *   read data from TAP interface.
                *   send that data by UDP socket
                */
                recvLen = read(tapfd, buffer, BUFFER_SIZE);
                CHECK_IF(0>recvLen, goto _END,
                         "tapfd read failed, recvLen = %d", recvLen);

                sendLen = udp_send(udpfd, buffer, recvLen);
                CHECK_IF(0>sendLen, goto _END,
                         "udp socket sendto failed, sendLen = %d", sendLen);
            }

            if (FD_ISSET(udpfd, &readset))
            {
                /*
                *   recv data from UDP socket
                *   write that data to TAP interface
                */
                recvLen = udp_recv(udpfd, buffer, BUFFER_SIZE);
                CHECK_IF(0>recvLen, goto _END,
                         "udp socket recv failed, recvLen = %d", recvLen);

                sendLen = write(tapfd, buffer, recvLen);
                CHECK_IF(0>sendLen, goto _END,
                         "tap write failed, sendLen = %d", sendLen);
            }
        }
    }

_END:
    if (udpfd > 0)
    {
        close(udpfd);
    }

    if (tapfd > 0)
    {
        close(tapfd);
    }

    FREE_ALL(ipaddr, netmask, dstip, ifname);

    return 0;
}
