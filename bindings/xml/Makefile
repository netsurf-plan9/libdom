ifeq ($(WITH_LIBXML_BINDING),yes)
  DIR_SOURCES := xmlparser.c

  DIR_INSTALL_ITEMS := /include/dom/bindings/libxml:xmlbinding.h;xmlerror.h;xmlparser.h

  # LibXML2
  ifneq ($(PKGCONFIG),)
    CFLAGS := $(CFLAGS) $(shell $(PKGCONFIG) libxml-2.0 --cflags)
    LDFLAGS := $(LDFLAGS) $(shell $(PKGCONFIG) libxml-2.0 --libs)
  else
    CFLAGS := $(CFLAGS) -I$(PREFIX)/include/libxml2
    LDFLAGS := $(LDFLAGS) -lxml2
  endif

  # LibXML 2.6.26 has a bug in its headers that expects _POSIX_C_SOURCE to be 
  # defined. Define it here, even though we don't need it.
  CFLAGS := $(CFLAGS) -D_POSIX_C_SOURCE
endif

include build/makefiles/Makefile.subdir

