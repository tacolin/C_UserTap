CC = gcc
#CC = /home/taco/CAPWAP_ARC_repos/openwrt/ac_from_arcadyan/arc_external/tools-gcc-4.7/bin/mips64-octeon-linux-gnu-gcc
#CC = /home/taco/CAPWAP_ARC_repos/toolchain_wtp/toolchain/build/gcc-4.3.3/build_mips/staging_dir/usr/bin/mips-linux-uclibc-gcc
CFLAGS =
OBJS = main.o tap.o udp.o
TARGET = usertap

default: ${OBJS}
	@${CC} -o ${TARGET} ${CFLAGS} ${OBJS}
clean:
	@rm -f ${TARGET} ${OBJS}
