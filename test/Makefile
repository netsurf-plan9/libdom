testutils_files := testutils/comparators.c;testutils/list.c;testutils/domasserts.c
testutils_files := $(testutils_files);testutils/utils.c;testutils/foreach.c;testutils/load.c

TESTCFLAGS := $(TESTCFLAGS) -I$(CURDIR) -I$(CURDIR)/testutils

DIR_TEST_ITEMS := $(testutils_files)

define add_xml_test
  ifeq ($$(WANT_TEST),yes)
	$(DIR)xml/c/$1.c: $(DIR)xml/tests/$1.xml
	$$(Q)$$(XSLTPROC) $$(XSLTPROCFLAGS) -o $$@ \
		$(DIR)transform/test-to-c.xsl $$<

	DIR_TEST_ITEMS := $$(DIR_TEST_ITEMS) $1:xml/c/$1.c;$$(testlib_files)
  endif

  DISTCLEAN_ITEMS := $$(DISTCLEAN_ITEMS) $(DIR)xml/c/$1.c

endef

# Add DOMTS tests to the list of items to build
$(eval $(foreach XML,$(wildcard $(DIR)xml/tests/*.xml), \
	$(call add_xml_test,$(basename $(notdir $(XML))))))

include build/makefiles/Makefile.subdir

