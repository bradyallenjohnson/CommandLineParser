SHELL=/bin/bash
CC=/usr/bin/g++
DELETE_CMD=/bin/rm -f

LIBRARY_PATH=
INCLUDE_PATH=-I.

OBJ_EXTENSION=.o
CCFLAGS=-g -fPIC
LDFLAGS=-shared

LIB_NAME=libCmdLineParser.so
OBJS=CmdLineParser$(OBJ_EXTENSION)

#
# Targets
#

all: $(LIB_NAME) $(OBJS)

$(LIB_NAME): $(OBJS)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(LIBRARY_PATH) -o $(LIB_NAME) $(OBJS)

CmdLineParser$(OBJ_EXTENSION): CmdLineParser.cc
	$(CC) $(CCFLAGS) $(INCLUDE_PATH) -c CmdLineParser.cc -o CmdLineParser$(OBJ_EXTENSION)

clean:
	$(DELETE_CMD) $(OBJS) $(EXE_NAME)
