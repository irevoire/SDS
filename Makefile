CC := gcc

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes
CFLAGS := -g -std=gnu99 $(WARNINGS)

EXE := main

all: $(EXE)

main: main.o stif.o
	@$(CC) $(CFLAGS) $^ -o $@

main.o: main.c
	@$(CC) $(CFLAGS) -c $<

stif.o: stif.c stif.h
	@$(CC) $(CFLAGS) -c $<

clean:
	@rm -f *.o $(EXE)
