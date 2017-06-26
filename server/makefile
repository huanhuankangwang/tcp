%.o : %.S
	$(CC) -g -c $^
	
%.o : %.c
	$(CC) -g $(CFLAGS) -c $^
	
OBJ:=../common/common.a
OBJS:= server.o

server:$(OBJS)
	$(CC) -o $@  $^ $(OBJ) $(CFLAGS)

clean:
	rm *.o -rf