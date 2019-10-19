EXE 		= wordcount
CC	 		= gcc
CCFLAGS 	= -Wall -Werror -pthread -O0 -g
LDFLAGS 	= -pthread

BIN			= bin
SOURCES		= $(wildcard *.c)
OBJECTS		= $(SOURCES:%.c=$(BIN)/%.o)
TESTS		= $(wildcard test/*.c)

.PHONY: all clean

all: $(BIN) $(EXE)

clean:
	rm -rf $(BIN)

$(BIN):
	mkdir -p $@

$(EXE): $(OBJECTS)
	$(CC) -o $(BIN)/$@ $^ $(LDFLAGS)
	@echo "Build successful"

$(OBJECTS): $(BIN)/%.o: %.c
	$(CC) -o $@ -c $< $(CCFLAGS)
