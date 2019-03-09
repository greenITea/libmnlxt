#!/bin/sh
clang-format -i include/libmnlxt/*.h src/*.c src/*.h src/rtnl/*.c src/xfrm/*.c tests/rtnl/*.c tests/xfrm/*.c
