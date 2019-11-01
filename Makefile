CC	 		= gcc
CXX			= g++
FLAGS		= -Wall -Werror -pthread -O2 -march=native
CCFLAGS 	= $(FLAGS) -std=c99
CXXFLAGS	= $(FLAGS) -std=c++11
LDFLAGS 	= -pthread -lstdc++

SOURCES		= distwc.c mapreduce.cpp threadpool.c
OBJECTS		= distwc.o mapreduce.o threadpool.o

.PHONY: all wc compile compress clean

all: wc

wc: wordcount

wordcount: $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

compile: $(OBJECTS)

distwc.o: distwc.c mapreduce.h
	$(CC) -o $@ -c $< $(CCFLAGS)

mapreduce.o: mapreduce.cpp mapreduce.h threadpool.h
	$(CXX) -o $@ -c $< $(CXXFLAGS)

threadpool.o: threadpool.c threadpool.h
	$(CC) -o $@ -c $< $(CCFLAGS)

compress:
	zip mapreduce.zip *.cpp *.c *.h Makefile README.md

clean:
	rm -f *.o wordcount
