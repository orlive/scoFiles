CC=cc
CFLAGS=-DNDEBUG

LIBS =	-lcurses -lc_s -nointl -lc -lintl

PROGS = \
		os	\
		rm1	\
		mouse

make:
	@make $(PROGS)

#==============================================================================#

OS_OBJ = \
		os.o		\
		os_dir.o	\
		os_be.o

OS_OBJA = \
		eingabe.o	\
		etc.o		\
		log.o		\
		match.o		\
		edit.o

os: $(OS_OBJ) $(OS_OBJA)
	$(CC) $(CFLAGS) -o os $(OS_OBJ) $(OS_OBJA) $(LIBS)

#==============================================================================#

TEST_OBJ = \
		rm1.o

TEST_OBJA = \
		etc.o		

rm1: $(TEST_OBJ) $(TEST_OBJA)
	$(CC) $(CFLAGS) -o rm1 $(TEST_OBJ) $(TEST_OBJA) $(LIBS)

#==============================================================================#

MOUSE_OBJ = \
		mouse.o

mouse: $(MOUSE_OBJ)
	$(CC) $(CFLAGS) -o mouse $(MOUSE_OBJ) $(LIBS)

#==============================================================================#
