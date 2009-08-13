# Component settings
COMPONENT := dom
COMPONENT_VERSION := 0.0.1
# Default to a static library
COMPONENT_TYPE ?= lib-static

# Setup the tooling
include build/makefiles/Makefile.tools

TESTRUNNER := $(PERL) build/testtools/testrunner.pl

# Toolchain flags
WARNFLAGS := -Wall -Wextra -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes \
	-Wmissing-declarations -Wnested-externs -Werror -pedantic
CFLAGS := -std=c99 -D_BSD_SOURCE -I$(CURDIR)/include/ \
	-I$(CURDIR)/src $(WARNFLAGS) $(CFLAGS)

# Parserutils & wapcaplet
ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
  ifneq ($(PKGCONFIG),)
    CFLAGS := $(CFLAGS) $(shell $(PKGCONFIG) libparserutils --cflags)
    CFLAGS := $(CFLAGS) $(shell $(PKGCONFIG) libwapcaplet --cflags)
    LDFLAGS := $(LDFLAGS) $(shell $(PKGCONFIG) libparserutils --libs)
    LDFLAGS := $(LDFLAGS) $(shell $(PKGCONFIG) libwapcaplet --libs)
  else
    CFLAGS := $(CFLAGS) -I$(PREFIX)/include
    LDFLAGS := $(LDFLAGS) -lparserutils -lwapcaplet
  endif
endif

include build/makefiles/Makefile.top

# Extra installation rules
Is := include/dom
I := /include/dom
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/dom.h;$(Is)/functypes.h

Is := include/dom/bootstrap
I := /include/dom/bootstrap
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/implpriv.h;$(Is)/implregistry.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/init_fini.h

Is := include/dom/core
I := /include/dom/core
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/attr.h;$(Is)/characterdata.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/document.h;$(Is)/document_type.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/element.h;$(Is)/exceptions.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/implementation.h;$(Is)/impllist.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/namednodemap.h;$(Is)/node.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/nodelist.h;$(Is)/string.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/text.h;$(Is)/typeinfo.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/comment.h

Is := include/dom/events
I := /include/dom/events
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/event.h;$(Is)/ui_event.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/custom_event.h;$(Is)/mouse_event.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/keyboard_event.h;$(Is)/text_event.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/mouse_wheel_event.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/mouse_multi_wheel_event.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/mutation_event.h;$(Is)/event_target.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/mutation_name_event.h;$(Is)/events.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/event_listener.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/document_event.h

INSTALL_ITEMS := $(INSTALL_ITEMS) /lib/pkgconfig:lib$(COMPONENT).pc.in
INSTALL_ITEMS := $(INSTALL_ITEMS) /lib:$(OUTPUT)

ifeq ($(WITH_LIBXML_BINDING),yes)
  REQUIRED_PKGS := $(REQUIRED_PKGS) libxml-2.0
endif

ifeq ($(WITH_HUBBUB_BINDING),yes)
  REQUIRED_PKGS := $(REQUIRED_PKGS) libhubbub
endif

