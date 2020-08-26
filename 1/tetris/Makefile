CC=cc
CFLAGS=-DNDEBUG

LIBS =	-lcurses -lc_s -nointl -lc

PROGS = \
		tetris

make:
	@make $(PROGS)

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
