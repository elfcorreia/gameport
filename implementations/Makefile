all:
	@for d in `find . -name "Makefile" -not -path './Makefile'`; do   \
		$(MAKE) -C `dirname $$d`; 				\
	done