ifeq ($(WITH_LIBXML_BINDING),yes)
  DIR_SOURCES := xmlbinding.c xmlparser.c

  DIR_INSTALL_ITEMS := /include/dom/bindings/libxml:xmlbinding.h;xmlerror.h;xmlparser.h

  # LibXML2
  ifneq ($(PKGCONFIG),)
    CFLAGS := $(CFLAGS) $(shell $(PKGCONFIG) libxml-2.0 --cflags)
    LDFLAGS := $(LDFLAGS) $(shell $(PKGCONFIG) libxml-2.0 --libs)
  else
    LDFLAGS := $(LDFLAGS) -lxml2
  endif
endif

include build/makefiles/Makefile.subdir

