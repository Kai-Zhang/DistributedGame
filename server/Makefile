CC          = g++
LD          = ld
CPPFLAGS    = -MD -fno-strict-aliasing -std=c++11 -g -I./include
CFILES      = $(shell find . -name "*.cpp")
HEADERS     = $(shell find . -name "*.h")
OBJS        = $(CFILES:.cpp=.o)
MAIN        = server
PORT        = 60000

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

-include $(patsubst %.o, %.d, $(OBJS))

.PHONY: clean run gdb

clean:
	-rm -f $(MAIN) $(OBJS) $(OBJS:.o=.d) *~ 2> /dev/null

run:
	./$(MAIN) $(PORT)

gdb:
	-@gdb ./$(MAIN) $(PORT)
