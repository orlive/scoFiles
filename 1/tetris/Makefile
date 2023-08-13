CC=cc
CFLAGS=-DNDEBUG -Wimplicit-function-declaration -Wincompatible-pointer-types

LIBS =	-lcurses -lc

PROGS = \
		tetris

make:
	@make $(PROGS)

cleanall:
	@rm *.o; make


#==============================================================================#

TET_OBJ = \
		tetris.o	\
		tetris_h.o	

TET_OBJA = \
		eingabe.o	\
		etc.o

tetris: $(TET_OBJ) $(TET_OBJA)
	$(CC) $(CFLAGS) -o tetris $(TET_OBJ) $(TET_OBJA) $(LIBS)

#==============================================================================#
