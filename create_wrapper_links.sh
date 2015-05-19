#!/bin/sh

CROSS_PREFIX="powerpc64-unknown-linux-gnu-"
HOST_PREFIX="x86_64-pc-linux-gnu-"
GCC_TOOLS="gcc g++ ld objdump readelf nm gcov ar as cpp objcopy"

WRAPPERS="yacc bison lex flex pahole"

for t in ${GCC_TOOLS}
do
    WRAPPERS+=" ${HOST_PREFIX}$t"
    WRAPPERS+=" ${CROSS_PREFIX}$t"
done

for w in ${WRAPPERS}
do
    ln -s mchroot_wrapper.sh $w
done

ln -s jail_wrapper.sh ${HOST_PREFIX}jail
