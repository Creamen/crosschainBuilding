#!/bin/bash

source inc/config.inc
source inc/functions.inc

[ -f ".config_override.inc" ] && source .config_override.inc

lockBuild

initCheck
initContext "$1"
initPath

initArchives
unpackArchives

### Here we go
# Step (0) = Initial setup
if [ ! -f .gcc.alpha.done ]; then
(
echo "[$(date -Iminutes)] Initial setup ..."

cd gcc-${compoments[gcc.ver]}
for dep in mpfr gmp mpc isl cloog; do
	ln -vsf ../${dep}-${compoments[${dep}.ver]} ${dep}
done
cd ..
) | tee /proc/self/fd/2 >> .${ts_log}_gcc.log  && touch .gcc.alpha.done || rm -f .gcc.alpha.done
else
	echo "[$(date -Iminutes)] Initial setup is up-to-date ... skipping !"
fi

# Step (1) = Binutils
if [ ! -f .binutils.0.done ]; then
(
echo "[$(date -Iminutes)] Build binutils ..."
cd build-binutils
../binutils-${compoments[binutils.ver]}/configure --prefix=${HOME}/cross/ --target=${TARGET} --disable-multilib
make -j6
make install
cd ..
) | tee /proc/self/fd/2 >> .${ts_log}_binutils.log  && touch .binutils.0.done || rm -f .binutils.0.done
else
	echo "[$(date -Iminutes)] binutils is up-to-date ... skipping !"
fi

# Step (2) = Kernel Headers
if [ ! -f .kernelheaders.0.done ]; then
(
echo "[$(date -Iminutes)] Install Linux kernel headers ..."
cd linux-${compoments[linux.ver]}
make ARCH=${KERNEL_ARCH} INSTALL_HDR_PATH=${PREFIX} headers_install
cd ..
) | tee /proc/self/fd/2 >> .${ts_log}_kernelheaders.log && touch .kernelheaders.0.done || rm -f .kernelheaders.0.done
else
	echo "[$(date -Iminutes)] kernel headers are up-to-date ... skipping !"
fi

# Step (3) = C/C++ Compilers
if [ ! -f .gcc.0.done ]; then
(
cd build-gcc
../gcc-${compoments[gcc.ver]}/configure --prefix=${HOME}/cross --target=${TARGET} --enable-languages=c,c++,go --disable-multilib --enable-tls --with-pkgversion=Creamen.NET --enable-threads --enable-nls
make -j6 all-gcc
make install-gcc
cd ..
) | tee /proc/self/fd/2 >> .${ts_log}_gcc.log && touch .gcc.0.done || rm -f .gcc.0.done
else
	echo "[$(date -Iminutes)] gcc step 0 is up-to-date ... skipping !"
fi

# Step (4) = Standard C Library Headers and Startup Files
if [ ! -f .glibc.0.done ]; then
(
cd build-glibc
../glibc-${compoments[glibc.ver]}/configure --prefix=${PREFIX} --build=${MACHTYPE} --host=${TARGET} --target=${TARGET} --with-headers=${PREFIX}/include --disable-multilib libc_cv_forced_unwind=yes
make install-bootstrap-headers=yes install-headers
make -j6 csu/subdir_lib
install csu/crt1.o csu/crti.o csu/crtn.o ${PREFIX}/lib
${MULTIARCH_NAME}-gcc -nostdlib -nostartfiles -shared -x c /dev/null -o ${PREFIX}/lib/libc.so
#mkdir -p ${PREFIX}/include/gnu/
touch ${PREFIX}/include/gnu/stubs.h
cd ..
) | tee /proc/self/fd/2 >> .${ts_log}_glibc.log && touch .glibc.0.done || rm -f .glibc.0.done
else
	echo "[$(date -Iminutes)] glibc step 0 is up-to-date ... skipping !"
fi

# Step (5) = Compiler Support Library
if [ ! -f .gcc.1.done ]; then
(
cd build-gcc
make -j6 all-target-libgcc
make install-target-libgcc
cd ..
) | tee /proc/self/fd/2 >> .${ts_log}_gcc.log && touch .gcc.1.done || rm -f .gcc.1.done
else
	echo "[$(date -Iminutes)] gcc step 1 is up-to-date ... skipping !"
fi

# Step (6) = Standard C Library
if [ ! -f .glibc.1.done ]; then
(
cd build-glibc
make -j6
make install
cd ..
) | tee /proc/self/fd/2 >> .${ts_log}_glibc.log && touch .glibc.1.done || rm -f .glibc.1.done
else
	echo "[$(date -Iminutes)] glibc step 1 is up-to-date ... skipping !"
fi

# Step (7) = Standard C++ Library
if [ ! -f .gcc.2.done ]; then
(
cd build-gcc
make -j6
make install
cd ..
) | tee /proc/self/fd/2 >> .${ts_log}_gcc.log && touch .gcc.2.done || rm -f .gcc.2.done
else
	echo "[$(date -Iminutes)] gcc step 2 is up-to-date ... skipping !"
fi
### curl -LO $(p=bash && e=gz && echo ftp://ftp.gnu.org/gnu/$p/$(lftp ftp://ftp.gnu.org/gnu/$p/ <<< "ls -tr $p*.$e" | tail -1 | grep -oE "$p.+"))

echo
echo "Your cross-build chain is available in ${PREFIX} ."
echo "Add ${HOME}/cross/bin to your PATH env like that :"
echo -e "\texport PATH=${HOME}/cross/bin:\${PATH}"
echo

echo "Build hello.c for your target architecture :"
cd - > /dev/null
cd test
${HOME}/cross/bin/${MULTIARCH_NAME}-gcc hello.c -o ${MULTIARCH_NAME}-hello
cd ..
ls test/${MULTIARCH_NAME}-hello 

exit 0

