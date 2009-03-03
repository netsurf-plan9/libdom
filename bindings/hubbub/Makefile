# Makefile for libdom
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
CFLAGS += -I$(CURDIR) `$(PKGCONFIG) --cflags libhubbub`

# Release output
RELEASE = ${TOP}/${COMPONENT}-libhubbub.a

# Debug output
DEBUG = ${TOP}/${COMPONENT}-libhubbub-debug.a

# Objects
OBJS = parser

.PHONY: clean debug distclean export release setup test

# Targets
release: $(addprefix Release/, $(addsuffix .o, $(OBJS)))
	@${AR} ${ARFLAGS} $(RELEASE) Release/*

debug: $(addprefix Debug/, $(addsuffix .o, $(OBJS)))
	@${AR} ${ARFLAGS} $(DEBUG) Debug/*

clean:
	-@${RM} ${RMFLAGS} $(addprefix Release/, $(addsuffix .o, ${OBJS}))
	-@${RM} ${RMFLAGS} $(addprefix Debug/, $(addsuffix .o, ${OBJS}))
	-@${RM} ${RMFLAGS} $(RELEASE) $(DEBUG)

distclean:
	-@${RM} ${RMFLAGS} -r Release
	-@${RM} ${RMFLAGS} -r Debug

setup:
	@${MKDIR} ${MKDIRFLAGS} Release
	@${MKDIR} ${MKDIRFLAGS} Debug

export:
	@${CP} ${CPFLAGS} $(RELEASE) ${EXPORT}/lib/

test:

# Pattern rules
Release/%.o: %.c
	@${ECHO} ${ECHOFLAGS} "==> $<"
	@${CC} -c ${CFLAGS} -DNDEBUG -o $@ $<

Debug/%.o: %.c
	@${ECHO} ${ECHOFLAGS} "==> $<"
	@${CC} -c -g ${CFLAGS} -o $@ $<

