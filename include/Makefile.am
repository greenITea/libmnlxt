
pkginclude_HEADERS = libmnlxt/mnlxt.h libmnlxt/core.h libmnlxt/data.h

if ENABLE_RTM
  pkginclude_HEADERS += libmnlxt/rt.h libmnlxt/rt_addr.h libmnlxt/rt_link.h libmnlxt/rt_route.h libmnlxt/rt_rule.h
endif

if ENABLE_XFRM
  pkginclude_HEADERS += libmnlxt/xfrm.h libmnlxt/xfrm_policy.h
endif

pkginclude_HEADERS += libmnlxt/features.h
CLEANFILES = libmnlxt/features.h

libmnlxt/features.h:
	$(MKDIR_P) $(@D)
	echo '#ifndef LIBMNLXT_FEATURES_H_' >$@
	echo '#define LIBMNLXT_FEATURES_H_' >>$@
if ENABLE_RTM
	echo '#define LIBMNLXT_WITH_RTM' >>$@
endif
if ENABLE_XFRM
	echo '#define LIBMNLXT_WITH_XFRM' >>$@
endif
	echo '#endif /* LIBMNLXT_FEATURES_H_ */' >>$@
