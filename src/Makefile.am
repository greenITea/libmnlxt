lib_LTLIBRARIES = libmnlxt.la

libmnlxt_la_SOURCES  = mnlxt.c mnlxt_data.c rtnl/rtnl.c
libmnlxt_la_SOURCES += rtnl/addr.c rtnl/addr_data.c
libmnlxt_la_SOURCES += rtnl/link.c rtnl/link_data.c
libmnlxt_la_SOURCES += rtnl/route.c rtnl/route_data.c
libmnlxt_la_SOURCES += rtnl/rule.c rtnl/rule_data.c
libmnlxt_la_SOURCES += xfrm/xfrm.c xfrm/policy.c xfrm/policy_data.c

libmnlxt_la_CFLAGS = -I$(top_srcdir)/include -I./ -Wall -Werror

#Here are a set of rules to help you update your library version information:
#
# * Start with version information of ‘0:0:0’ for each libtool library.
# * Update the version information only immediately before a public release of your software. 
#  More frequent updates are unnecessary, and only guarantee that the current interface number gets larger faster.
# * If the library source code has changed at all since the last update, then increment revision (‘c:r:a’ becomes ‘c:r+1:a’).
# * If any interfaces have been added, removed, or changed since the last update, increment current, and set revision to 0.
# * If any interfaces have been added since the last public release, then increment age.
# * If any interfaces have been removed or changed since the last public release, then set age to 0.

libmnlxt_la_LDFLAGS = -version-info 0:0:0

