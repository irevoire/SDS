# Main makefile

SUBDIR := src regress tests

OBJDIR := obj
BINDIR := bin

export # allow all variables to be inclued in the sub Makefile

all:
	@for dir in ${SUBDIR} ; do \
		echo "[*] Building subdir $$dir" ; \
		$(MAKE) -C $$dir || exit ;\
	done

.PHONY: clean testu

test: CFLAGS += --coverage -O0
test: LDFLAGS += -fprofile-arcs -ftest-coverage
test: all
	./bin/check_main
#	@for dir in ${SUBDIR} ; do \
		for file in $$dir/*.c; do \
			gcov $$dir/$$file -o ${OBJDIR}/$$file;\
		done \
	done
#	lcov -d bin --zerocounters
	lcov -d . -c -o report.info
#	genhtml -o ./coverage report.info 

clean:
	@echo "[*] Cleaning"
	rm -rf ${OBJDIR} ${BINDIR} *.gcno *.gcov coverage
