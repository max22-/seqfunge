LDFLAGS = -lnotcurses -lnotcurses-core

all: seqfunge
seqfunge: seqfunge.c
	gcc seqfunge.c -o seqfunge -Wall $(LDFLAGS)

.PHONY: run clean

run: seqfunge
	./seqfunge

clean:
	rm -f seqfunge
