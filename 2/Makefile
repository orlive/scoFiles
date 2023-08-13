CC=cc
CFLAGS=-DNDEBUG -Wimplicit-function-declaration

LIBS =	-lcurses -lc

PROGS = \
		remo

make:
	@make $(PROGS)

#==============================================================================#

OBJ = remo.o

tetris: $(OBJ)
	$(CC) $(CFLAGS) -o remo $(OBJ) $(LIBS)

#==============================================================================#
