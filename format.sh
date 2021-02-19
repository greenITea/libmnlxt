#!/bin/sh
clang-format -i include/libmnlxt/*.h include/private/*.h src/*.c src/rtnl/*.c src/xfrm/*.c tests/rtnl/*.c tests/xfrm/*.c
