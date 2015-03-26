CC		= gcc
LD		= ld
CFLAGS	= -MD -fno-strict-aliasing -std=c99 -g -I./*.h
CFILES	= $(shell find . -name "*.c")
OBJS	= $(CFILES:.c=.o)
MAIN	= client

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

-include $(OBJS:.o=.d)

.PHONY: clean run gdb

clean:
	-rm -f $(MAIN) $(OBJS) $(OBJS:.o=.d) *~ 2> /dev/null

run:
	./$(MAIN)

gdb:
	-@gdb ./$(MAIN)
