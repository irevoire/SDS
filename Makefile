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

all_test: test integration

test: CFLAGS += --coverage -O0
test: LDFLAGS += -fprofile-arcs -ftest-coverage
test: | clean all
	@echo "[*] Executing the tests to generate the gc files"
	@./bin/check_main
	@mkdir coverage
	@echo "[*] Moving gc file to coverage directory"
	@cp $$(find . -name "*.c") coverage/
	@mv $$(find . -name "*.gc*") coverage/
	@lcov -d . -c -o coverage/report.info
	@genhtml -o coverage coverage/report.info
	@echo "[*] A graphical coverage report is available in coverage/index.html"

integration: all
	@echo "[*] Checking all the checksum"
	@for i in images/*.stif; do \
		./bin/spcheck -i $$i -d | shasum | diff $$i.checksum - || (echo $$i " failed" && exit) ; \
	done
	@echo "[*] All integration test are ok"

checksum: all
	@echo "[*] Generating checksum"
	@for i in images/*.stif; do \
		./bin/spcheck -i $$i -d | shasum > $$i.checksum ;\
	done

clean:
	@echo "[*] Cleaning"
	@rm -rf ${OBJDIR} ${BINDIR} *.gcno *.gcov coverage
