#!/bin/bash

Toolchain=$1
cd $(dirname $0)
cd ..

if [ -z "${Toolchain}" ]; then
  Toolchain=unix
fi

curdir=$(pwd)
mkdir -p ${curdir}/bld
mkdir -p ${curdir}/bld/${Toolchain}

if [ ! -d $curdir/thirdparty/lib/${Toolchain} ]; then
  bash $curdir/thirdparty/install.sh ${Toolchain}
fi

if [ "${Toolchain}" strequal "mac" ]; then
  cmake -DCMAKE_TOOLCHAIN_FILE=toolchain/${Toolchain}-toolchain.cmake -B bld/${Toolchain}  .
  cmake --build bld/${Toolchain} --config release
else
  cmake -DCMAKE_TOOLCHAIN_FILE=toolchain/${Toolchain}-toolchain.cmake -DCMAKE_BUILD_TYPE=Release -B bld/${Toolchain}  .
  cmake --build bld/${Toolchain}
fi
cmake --install bld/${Toolchain} --prefix ${curdir}/bin/${Toolchain}
