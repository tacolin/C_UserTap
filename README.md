User Space TAP Tunnel Example
=============================

This is an example of TAP tunnel in user space.

File Descriptions
-----------------

| File   | Description                                   | 
|--------|-----------------------------------------------|
| main.c | main functions                                |
| udp.c  | udp socket creation, sending / receiving data |
| tap.c  | tap interface creation and configuration      |

How to Test?
------------

             COMPUTER A                           COMPUTER B
        192.186.1.1(10.10.10.1)              192.168.1.2(10.10.10.2)

(1) Create the TAP virutal network device in COMPUTER A and B

In COMPUTER A:

    $ sudo ./userapp --ip=10.10.10.1 --mask=255.255.255.0 --dst=192.168.1.2

In COMPUTER B:

    $ sudo ./userapp --ip=10.10.10.2 --mask=255.255.255.0 --dst=192.168.1.1

(2) Ping by TAP Tunnel (open another console)

In COMPUTER A:

    $ ping 10.10.10.2

In COMPUTER B:

    $ ping 10.10.10.1


References
----------
[1.Example of TUN in User Space](http://neokentblog.blogspot.tw/2014/05/linux-virtual-interface-tuntap.html)
