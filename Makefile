
EXE 		= wordcount
CXX 		= g++
CXXFLAGS 	= -Wall -Werror -std=c++14 -O0 -g
LDFLAGS 	= -pthread

BIN			= bin
SOURCES		= $(wildcard *.cpp)
OBJECTS		= $(SOURCES:%.cpp=$(BIN)/%.o)

.PHONY: all clean

all: $(BIN) $(BIN)/$(EXE)

clean:
	rm -rf $(BIN)

$(BIN):
	mkdir -p $@

$(BIN)/$(EXE): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	@echo "Build successful"

$(OBJECTS): $(BIN)/%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)