
PROJECT_ROOT = ../../
SRC_DIR = $(PROJECT_ROOT)src/
TESTS_DIR = $(PROJECT_ROOT)tests/
BIN_DIR = $(PROJECT_ROOT)bin/
INCLUDES = -I$(PROJECT_ROOT)src

CXX = g++
CC = gcc
MPICC = mpic++
CFLAGS = -Wall -g -lstdc++ -std=c++11 -O3 -pthread $(INCLUDES)
CCFLAGS = -Wall -g -lstdc++ -O3 $(INCLUDES)

UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
	CCFLAGS += -arch x86_64
	CFLAGS += -arch x86_64
endif

CFLAGS_MP = $(CFLAGS)

SRC_TEST = $(TESTS_DIR)main.cpp $(TESTS_DIR)test_dna.cpp


AIRCRACK_DIR = $(PROJECT_ROOT)src/aircrack/
SRC_AIRCRACK = $(AIRCRACK_DIR)crypto.c $(AIRCRACK_DIR)aircrack-ptw-lib.c

OBJS_TEST = $(SRC_TEST:.cpp=.o)

TESTS_BIN = tests

.PHONY: depend clean

all:	$(TESTS_BIN)
	@echo  Simple compiler named mycc has been compiled
	@echo $(UNAME_S)
#	./$(BIN_DIR)$(TESTS_BIN)

$(TESTS_BIN): $(OBJS_TEST) 
	$(CXX) $(CFLAGS) -o $(BIN_DIR)$(TESTS_BIN) $(OBJS_TEST)

.cpp.o:
	$(CXX) $(CFLAGS_MP) -c $<  -o $@

.c.o:
	$(CC) $(CCFLAGS) -c $<  -o $@

clean:
	$(RM) $(SRC_DIR)*.o $(TESTS_DIR)*.o *~ $(TESTS_BIN)

depend: $(SRC_TEST)
	makedepend $(INCLUDES) $^

