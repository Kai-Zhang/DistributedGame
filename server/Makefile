CC		= gcc
LD		= ld
CFLAGS	= -MD -fno-strict-aliasing -std=c99 -g -I./*.h -pthread
CFILES	= $(shell find . -name "*.c")
OBJS	= $(CFILES:.c=.o)
MAIN	= server

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

-include $(patsubst %.o, %.d, $(OBJS))

.PHONY: clean run gdb

clean:
	-rm -f $(MAIN) $(OBJS) $(OBJS:.o=.d) *~ 2> /dev/null

run:
	./$(MAIN)

gdb:
	-@gdb ./$(MAIN)
