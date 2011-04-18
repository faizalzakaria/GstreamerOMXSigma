#!/bin/sh
# set -e
#

VERSION=0.25
SRC_DIR=pkg-config-$VERSION
cd $SRC_DIR

CC="mips-linux-gnu-gcc -EL" \
CFLAGS="-fPIC -march=mips32r2 -Wa,-mips32r2 -g" \
LDFLAGS="-L$P_INSTALL_DIR/lib \
	-L$SMP86XX_ROOTFS_PATH/cross_rootfs/lib" \
./configure --build=i486-linux-gnu \
            --host=mips-linux-gnu \
            --target=mips-linux-gnu \
            --prefix=$P_INSTALL_DIR \
            --with-installed-glib \

make || exit 1
make install

exit 0
