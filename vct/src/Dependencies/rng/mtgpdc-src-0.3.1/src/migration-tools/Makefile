#for GNU make

#DDEBUG = -O0 -ggdb
#DDEBUG = -O1 -g
#DDEBUG = -DDEBUG

# uncomment next line if you installed NTL with gf2x
#LIBGF2X = -lgf2x
# uncomment next line if you installed NTL with gmp
#LIBGMP = -lgmp

DEFINES = -D__STDC_CONSTANT_MACROS=1
CPP = g++ -Wall -Wextra -O3 -I../../include -I.. -msse3 $(DEFINES) $(DDEBUG)

LINKOPT = -lntl -lcrypto $(LIBGF2X) $(LIBGMP)
HEADERS = ../../include/linear_generator.hpp \
	../../include/random_util.hpp \
	../../include/search_temper.hpp \
	../../include/shortest_basis.hpp \
	../mtgp32.hpp

all:migration32

migration32: migration32.cpp ${HEADERS}
	${CPP} -o $@ migration32.cpp ${LINKOPT}

.cpp.o:
	${CPP} -c $<

clean:
	rm -rf *.o *~ *.dSYM
