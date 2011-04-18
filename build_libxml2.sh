#!/bin/sh
set -e
#

VERSION=2.7.7
SRC_DIR=libxml2-$VERSION
cd $SRC_DIR

# These two flags are needed to compile
CC="mips-linux-gnu-gcc -EL" \
CXX="mips-linux-gnu-g++ -EL" \
CXXFLAGS="-fPIC -march=mips32r2 -Wa,-mips32r2 -g" \
LDFLAGS="-L$P_INSTALL_DIR/lib \
	-L$SMP86XX_ROOTFS_PATH/cross_rootfs/lib" \
LIBXML2_HOST_BINARY="$P_INSTALL_DIR/bin/xmllint" \
./configure --build=i486-linux-gnu \
			--host=mips-linux-gnu \
			--target=mips-linux-gnu \
			--prefix=$P_INSTALL_DIR \
            --with-gnu-ld \
            --enable-shared \
            --enable-static \
            --without-python \
            --without-threads

make || exit 1
make install

exit 0
