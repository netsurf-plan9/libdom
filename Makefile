# Component settings
COMPONENT := dom
COMPONENT_VERSION := 0.0.1
# Default to a static library
COMPONENT_TYPE ?= lib-static

# Setup the tooling
include build/makefiles/Makefile.tools

TESTRUNNER := $(PERL) build/testtools/testrunner.pl

# Toolchain flags
WARNFLAGS := -Wall -W -Wundef -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes \
	-Wmissing-declarations -Wnested-externs -pedantic
# BeOS/Haiku standard library headers create warnings
ifneq ($(TARGET),beos)
  WARNFLAGS := $(WARNFLAGS) -Werror
endif
# AmigaOS needs this to avoid warnings
ifeq ($(TARGET),amiga)
  CFLAGS := -U__STRICT_ANSI__ $(CFLAGS)
endif
CFLAGS := -std=c99 -D_BSD_SOURCE -D_GNU_SOURCE -I$(CURDIR)/include/ \
	-I$(CURDIR)/src -I$(CURDIR)/binding $(WARNFLAGS) $(CFLAGS)

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

Is := include/dom/core
I := /include/dom/core
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/attr.h;$(Is)/characterdata.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/cdatasection.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/comment.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/doc_fragment.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/document.h;$(Is)/document_type.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/entity_ref.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/element.h;$(Is)/exceptions.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/implementation.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/namednodemap.h;$(Is)/node.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/nodelist.h;$(Is)/string.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/pi.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/text.h;$(Is)/typeinfo.h

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

Is := include/dom/html
I := /include/dom/html
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/html_element.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/html_document.h
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/html_form_element.h

INSTALL_ITEMS := $(INSTALL_ITEMS) /lib/pkgconfig:lib$(COMPONENT).pc.in
INSTALL_ITEMS := $(INSTALL_ITEMS) /lib:$(OUTPUT)

ifeq ($(WITH_LIBXML_BINDING),yes)
  REQUIRED_PKGS := $(REQUIRED_PKGS) libxml-2.0
endif

ifeq ($(WITH_HUBBUB_BINDING),yes)
  REQUIRED_PKGS := $(REQUIRED_PKGS) libhubbub
endif

