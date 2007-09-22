# Makefile for DOM testcases
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
LDFLAGS += `${PKGCONFIG} ${PKGCONFIGFLAGS} --libs libxml-2.0`

# Libraries we link against
LIBS = -L./lib -ldomtest-debug -ldom-libxml-debug -ldom-debug 

# Release output
RELEASE =

# Debug output
DEBUG =

# Transformer input
XMLFILES = $(wildcard xml/tests/*.xml)

# Transformer output
CFILES = $(addprefix xml/c/, $(notdir $(XMLFILES:.xml=.c)))

# Objects
XMLOBJS = $(addprefix xml/bin/, $(notdir $(XMLFILES:.xml=)))
OTHEROBJS = binding test-list
OBJS = $(OTHEROBJS) $(XMLOBJS)

.PHONY: clean debug export release setup test transform

# Targets
release:
	@${MAKE} -C lib release

debug:
	@${MAKE} -C lib debug

clean:
	@${MAKE} -C lib clean
ifneq (${OBJS}, )
	-@${RM} ${RMFLAGS} $(addsuffix ${EXEEXT}, $(OTHEROBJS))
	-@${RM} ${RMFLAGS} -r xml/c/
	-@${RM} ${RMFLAGS} -r xml/bin/
endif

distclean:
	@${MAKE} -C lib distclean
	-@${RM} ${RMFLAGS} log

setup:
	@${MAKE} -C lib setup
	@${MKDIR} ${MKDIRFLAGS} $(CURDIR)/xml/c
	@${MKDIR} ${MKDIRFLAGS} $(CURDIR)/xml/bin

export:
	@${MAKE} -C lib export

test: release debug $(OBJS)
	@${PERL} testrunner.pl ${EXEEXT}

transform: $(CFILES)

# Pattern rules
xml/bin/%: xml/c/%.c
	@${ECHO} ${ECHOFLAGS} "==> $<"
	@${CC} -c -g ${CFLAGS} -o $@.o $<
	@${LD} -g -o $@ $@.o ${LDFLAGS} $(LIBS)
	@${RM} ${RMFLAGS} $@.o

xml/c/%.c: xml/tests/%.xml
	@${XSLT} ${XSLTFLAGS} -o $@ transform/test-to-c.xsl $<

%: %.c
	@${ECHO} ${ECHOFLAGS} "==> $<"
	@${CC} -c -g ${CFLAGS} -o $@.o $<
	@${LD} -g -o $@ $@.o ${LDFLAGS} $(LIBS)
	@${RM} ${RMFLAGS} $@.o

