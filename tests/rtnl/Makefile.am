AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include
AM_LDFLAGS = $(top_builddir)/src/libmnlxt.la

rtnl_dump_SOURCES = rtnl_common.c rtnl_dump.c

rtnl_linkaddr_get_SOURCES = rtnl_linkaddr_get.c

rtnl_defaultroute_get_SOURCES = rtnl_defaultroute_get.c

rtnl_addr_mod_SOURCES = rtnl_common.c rtnl_addr_mod.c

rtnl_listen_SOURCES = rtnl_common.c rtnl_listen.c

rtnl_link_updown_SOURCES = rtnl_link_updown.c

rtnl_link_xfrm_mod_SOURCES = rtnl_link_xfrm_mod.c

rtnl_link_tun_mod_SOURCES = rtnl_link_tun_mod.c

rtnl_route_mod_SOURCES = rtnl_common.c rtnl_route_mod.c

bin_PROGRAMS = rtnl_dump rtnl_linkaddr_get rtnl_defaultroute_get \
	rtnl_addr_mod rtnl_listen rtnl_link_updown rtnl_link_xfrm_mod \
	rtnl_link_tun_mod rtnl_route_mod
