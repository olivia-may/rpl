.PHONY: all test clean

all: rpl

rpl:
	gcc rpl.c -o rpl

clean:
	rm rpl
