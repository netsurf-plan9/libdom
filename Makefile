# Component settings
COMPONENT := dom
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
I := include/dom
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/dom.h;$(I)/functypes.h

I := include/dom/bootstrap
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/implpriv.h;$(I)/implregistry.h
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/init_fini.h

I := include/dom/core
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/attr.h;$(I)/characterdata.h
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/document.h;$(I)/document_type.h
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/element.h;$(I)/exceptions.h
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/implementation.h;$(I)/impllist.h
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/namednodemap.h;$(I)/node.h
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/nodelist.h;$(I)/string.h
INSTALL_ITEMS := $(INSTALL_ITEMS) /$(I):$(I)/text.h

INSTALL_ITEMS := $(INSTALL_ITEMS) /lib/pkgconfig:lib$(COMPONENT).pc.in
INSTALL_ITEMS := $(INSTALL_ITEMS) /lib:$(BUILDDIR)/lib$(COMPONENT)$(LIBEXT)

ifeq ($(WITH_LIBXML_BINDING),yes)
  REQUIRED_PKGS := $(REQUIRED_PKGS) libxml-2.0
endif

ifeq ($(WITH_HUBBUB_BINDING),yes)
  REQUIRED_PKGS := $(REQUIRED_PKGS) libhubbub
endif

