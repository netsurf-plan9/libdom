ifeq ($(WITH_HUBBUB_BINDING),yes)
  DIR_SOURCES := parser.c

  DIR_INSTALL_ITEMS := /include/dom/bindings/hubbub:errors.h;parser.h

  # Hubbub
  ifneq ($(PKGCONFIG),)
    CFLAGS := $(CFLAGS) $(shell $(PKGCONFIG) libhubbub-0 --cflags)
    LDFLAGS := $(LDFLAGS) $(shell $(PKGCONFIG) libhubbub-0 --libs)
  else
    LDFLAGS := $(LDFLAGS) -lhubbub0 -lparserutils0
  endif
endif

include build/makefiles/Makefile.subdir

