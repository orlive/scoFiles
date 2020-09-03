CC=g++
CFLAGS=-DNDEBUG -Wno-deprecated

LIBS =	-lcurses -lc 

PROGS = \
		os

make:
	@make $(PROGS)

#==============================================================================#

OS_OBJ =      \
		os.o      \
		os_dir.o  \
		os_be.o   \
		eingabe.o	\
		etc.o     \
		log.o     \
		match.o   \
		edit.o  \
		controller.o

os: $(OS_OBJ)
	$(CC) $(CFLAGS) -o os $(OS_OBJ) $(OS_OBJA) $(LIBS)

clear:
	rm -f *.o os;

cleanall: 
	rm -f *.o os; \
	make os

#==============================================================================#
