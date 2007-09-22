# Makefile for DOM testcase utility library
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

# Manipulate include paths
CFLAGS += -I$(TOP) -I$(CURDIR)

# Release output
RELEASE = libdomtest.a

# Debug output
DEBUG = libdomtest-debug.a

# Objects
OBJS = list testassert testobject utils

.PHONY: clean debug export release setup test

# Targets
release: $(addprefix Release/, $(addsuffix .o, $(OBJS)))
	@${AR} ${ARFLAGS} $(RELEASE) Release/*

debug: $(addprefix Debug/, $(addsuffix .o, $(OBJS)))
	@${AR} ${ARFLAGS} $(DEBUG) Debug/*

clean:
ifneq (${OBJS}, )
	-@${RM} ${RMFLAGS} $(addprefix Release/, $(addsuffix .o, ${OBJS}))
	-@${RM} ${RMFLAGS} $(addprefix Debug/, $(addsuffix .o, ${OBJS}))
endif
	-@${RM} ${RMFLAGS} $(RELEASE) $(DEBUG)

distclean:
	-@${RM} ${RMFLAGS} -r Release
	-@${RM} ${RMFLAGS} -r Debug

setup:
	@${MKDIR} ${MKDIRFLAGS} Release
	@${MKDIR} ${MKDIRFLAGS} Debug

export:

test:

# Pattern rules
Release/%.o: %.c
	@${ECHO} ${ECHOFLAGS} "==> $<"
	@${CC} -c ${CFLAGS} -DNDEBUG -o $@ $<

Debug/%.o: %.c
	@${ECHO} ${ECHOFLAGS} "==> $<"
	@${CC} -c -g ${CFLAGS} -o $@ $<

