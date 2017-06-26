CFLAGS := -lpthread -I$(shell pwd)/include/
CC:= gcc
LD:=ld


export CFLAGS CC LD

all:
	make -C common
	make -C client
	make -C server

clean:
	make clean -C common
	make clean -C client
	make clean -C server