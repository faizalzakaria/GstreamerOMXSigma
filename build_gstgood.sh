
#!/bin/bash

set -e

VERSION=0.10.26
SRC_DIR=gst-plugins-good-$VERSION
cd $SRC_DIR

# In order to use pkg-config to build gstreamer
export PKG_CONFIG_PATH="$P_INSTALL_DIR/lib/pkgconfig:$SMP86XX_ROOTFS_PATH/cross_rootfs/lib/pkgconfig"

CC="mips-linux-gnu-gcc -EL" \
CFLAGS="-fPIC -march=mips32r2 -Wa,-mips32r2 -g" \
LIBS="-lz -lpng" \
CPPFLAGS="-I$SMP86XX_ROOTFS_PATH/cross_rootfs/include" \
LDFLAGS="-L$P_INSTALL_DIR/lib \
	-L$SMP86XX_ROOTFS_PATH/cross_rootfs/lib" \
./configure --build=i486-linux-gnu \
			--host=mips-linux-gnu \
			--target=mips-linux-gnu \
            --prefix=$P_INSTALL_DIR \
            --enable-shared \
            --disable-static \
            --disable-valgrind \
            --enable-debug=yes  \
            --disable-x \
            --disable-shout2 

make || exit 1
make install

exit 0
