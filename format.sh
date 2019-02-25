#!/bin/sh
clang-format -i include/*.h include/libmnlxt/*.h src/*.c src/rtnl/*.c src/xfrm/*.c tests/rtnl/*.c tests/xfrm/*.c
