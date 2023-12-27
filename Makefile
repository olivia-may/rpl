.PHONY: all rpl test tests clean

all: rpl

rpl:
	gcc rpl.c -o rpl

clean:
	rm rpl
