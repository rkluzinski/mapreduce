CC	 		= gcc
CXX			= g++
FLAGS		= -Wall -Werror -pthread -O2 -march=native
CCFLAGS 	= $(FLAGS) -std=c99
CXXFLAGS	= $(FLAGS) -std=c++11
LDFLAGS 	= -pthread

SOURCES		= $(wildcard *.c *.cpp)
OBJECTS		= $(SOURCES:%.c=%.o)

.PHONY: all wc compile compress clean

all: wc

wc: wordcount

wordcount: $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

compile: $(OBJECTS)

distw.o: distw.c mapreduce.h
	$(CC) -o $@ -c $< $(CCFLAGS)

mapreduce.o: mapreduce.cpp mapreduce.h threadpool.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

threadpool.o: threadpool.c threadpool.h
	$(CC) -o $@ -c $< $(CCFLAGS)

compress:
	zip mapreduce.zip *.cpp *.c *.h Makefile README.md

clean:
	rm -f *.o wordcount
