OBJS = main.cpp

CC = g++

COMPILER_FLAGS = -w

LINKER_FLAGS = -lSDL2 -lSDL2_mixer

OBJ_NAME = gamepaddrums

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

install: gamepaddrums
	install -d /usr/bin
	install -m 755 gamepaddrums /usr/bin