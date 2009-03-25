testlib_files := lib/comparators.c;lib/list.c;lib/testassert.c;
testlib_files := $(testlib_files);lib/testobject.c;lib/utils.c

TESTCFLAGS := $(TESTCFLAGS) -I$(CURDIR)

DIR_TEST_ITEMS := binding:binding.c;$(testlib_files) \
		test-list:test-list.c;$(testlib_files)

include build/makefiles/Makefile.subdir

# TODO: XML suite
#
# Transformer input
#XMLFILES = $(wildcard xml/tests/*.xml)
#
# Transformer output
#CFILES = $(addprefix xml/c/, $(notdir $(XMLFILES:.xml=.c)))
#
# Objects
#XMLOBJS = $(addprefix xml/bin/, $(notdir $(XMLFILES:.xml=)))
#
#transform: $(CFILES)
#
# Pattern rules
#xml/bin/%: xml/c/%.c
#	@${ECHO} ${ECHOFLAGS} "==> $<"
#	@${CC} -c -g ${CFLAGS} -o $@.o $<
#	@${LD} -g -o $@ $@.o ${LDFLAGS} $(LIBS)
#	@${RM} ${RMFLAGS} $@.o
#
#xml/c/%.c: xml/tests/%.xml
#	@${XSLT} ${XSLTFLAGS} -o $@ transform/test-to-c.xsl $<

