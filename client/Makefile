CC		= gcc
LD		= ld
CFLAGS	= -MD -fno-strict-aliasing -std=c99 -g -I./include
CFILES	= $(shell find . -name "*.c")
OBJS	= $(CFILES:.c=.o)
MAIN	= client
SERVIP	= 127.0.0.1
PORT	= 60000

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

-include $(OBJS:.o=.d)

.PHONY: clean run gdb

clean:
	-rm -f $(MAIN) $(OBJS) $(OBJS:.o=.d) *~ 2> /dev/null

run:
	./$(MAIN) $(SERVIP) $(PORT)

gdb:
	-@gdb ./$(MAIN) $(SERVIP) $(PORT)
