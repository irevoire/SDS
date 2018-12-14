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

.PHONY: clean test all

test: CFLAGS += --coverage -O0
test: LDFLAGS += -fprofile-arcs -ftest-coverage
test: | clean all
	./bin/check_main
	mkdir coverage
	cp $$(find . -name "*.c") coverage/
	mv $$(find . -name "*.gc*") coverage/
	lcov -d . -c -o coverage/report.info
	genhtml -o coverage coverage/report.info

clean:
	@echo "[*] Cleaning"
	rm -rf ${OBJDIR} ${BINDIR} *.gcno *.gcov coverage
