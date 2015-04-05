CC          = g++
LD          = ld
CPPFLAGS    = -MD -fno-strict-aliasing -std=c++11 -g -I./include -pthread
CFILES      = $(shell find src/ -name "*.cpp")
OBJS        = $(CFILES:.cpp=.o)
MAIN        = server
PORT        = 60000

$(MAIN): $(OBJS)
	$(CC) $(CPPFLAGS) -o $(MAIN) $(OBJS)

fake: fake_client.cpp
	$(CC) $(CPPFLAGS) -o client fake_client.cpp
	./client 127.0.0.1 $(PORT)

-include $(patsubst %.o, %.d, $(OBJS))

.PHONY: clean run gdb

clean:
	-rm -f $(MAIN) $(OBJS) $(OBJS:.o=.d) *~ 2> /dev/null

run:
	./$(MAIN) $(PORT)

gdb:
	-@gdb --args ./$(MAIN) $(PORT)

fakedebug:
	-@gdb --args ./client 127.0.0.1 $(PORT)
