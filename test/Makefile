# Makefile for Hubbub testcases
#
# Toolchain is exported by top-level makefile
#
# Top-level makefile also exports the following variables:
#
# COMPONENT  Name of component
# EXPORT     Absolute path of export directory
# TOP        Absolute path of source tree root
#
# The top-level makefile requires the following targets to exist:
#
# clean      Clean source tree
# debug      Create a debug binary
# distclean  Fully clean source tree, back to pristine condition
# export     Export distributable components to ${EXPORT}
# release    Create a release binary
# setup      Perform any setup required prior to compilation
# test       Execute any test cases

# Extend toolchain settings
CFLAGS += -I${TOP}/src/ -I${TOP}/bindings/xml/ -I$(CURDIR)
LDFLAGS += `pkg-config --libs libxml-2.0`

# Release output
RELEASE =

# Debug output
DEBUG =

# Objects
OBJS = binding

.PHONY: clean debug export release setup test

# Targets
release:

debug:

clean:
ifneq (${OBJS}, )
	-@${RM} ${RMFLAGS} $(addsuffix ${EXEEXT},  $(OBJS))
endif

distclean:
	-@${RM} ${RMFLAGS} log

setup:

export:

test: $(OBJS)
	@${PERL} testrunner.pl ${EXEEXT}

# Pattern rules
%: %.c
	@${ECHO} ${ECHOFLAGS} "==> $<"
	@${CC} -c -g ${CFLAGS} -o $@.o $<
	@${LD} -g -o $@ $@.o ${LDFLAGS} -ldom-libxml-debug -ldom-debug
	@${RM} ${RMFLAGS} $@.o
