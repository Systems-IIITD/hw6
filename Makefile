SRC=$(wildcard *.c)
DST=$(patsubst %.c,%,$(SRC))

default: $(DST)

%: %.c
	gcc -O3 -o $@ $^ -lpthread

run: false
	time -p ./false

clean:
	rm -rf $(DST)
