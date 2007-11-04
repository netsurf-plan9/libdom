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

.PHONY: clean debug distclean export release setup test

# Targets
release:
	@${MAKE} -C xml release

debug: 
	@${MAKE} -C xml debug

clean:
	@${MAKE} -C xml clean
	
distclean:
	@${MAKE} -C xml distclean

setup:
	@${MAKE} -C xml setup

export:
	@${MAKE} -C xml export

test:
	@${MAKE} -C xml test

