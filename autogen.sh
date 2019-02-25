#!/bin/sh

autoheader && \
libtoolize -fci && \
aclocal && \
autoconf && \
automake -ac