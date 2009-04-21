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
CFLAGS := $(CFLAGS) -std=c99 -D_BSD_SOURCE -I$(CURDIR)/include/ \
	-I$(CURDIR)/src $(WARNFLAGS) 

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
INSTALL_ITEMS := $(INSTALL_ITEMS) $(I):$(Is)/text.h

INSTALL_ITEMS := $(INSTALL_ITEMS) /lib/pkgconfig:lib$(COMPONENT).pc.in
INSTALL_ITEMS := $(INSTALL_ITEMS) /lib:$(OUTPUT)

ifeq ($(WITH_LIBXML_BINDING),yes)
  REQUIRED_PKGS := $(REQUIRED_PKGS) libxml-2.0
endif

ifeq ($(WITH_HUBBUB_BINDING),yes)
  REQUIRED_PKGS := $(REQUIRED_PKGS) libhubbub
endif

