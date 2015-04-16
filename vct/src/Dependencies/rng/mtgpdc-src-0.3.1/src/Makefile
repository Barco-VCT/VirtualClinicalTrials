#for GNU make

#DDEBUG = -O0 -ggdb
#DDEBUG = -O1 -g
#DDEBUG = -DDEBUG

# uncomment next line if you installed NTL with gf2x
#LIBGF2X = -lgf2x
# uncomment next line if you installed NTL with gmp
#LIBGMP = -lgmp

DEFINES = -D__STDC_CONSTANT_MACROS=1
CPP = g++ -Wall -Wextra -O3 -I../include -msse3 $(DEFINES) $(DDEBUG)

LINKOPT = -lntl -lcrypto $(LIBGF2X) $(LIBGMP)
HEADERS = ../include/all_in_one.hpp \
	../include/linear_generator.hpp \
	../include/mt19937.hpp \
	../include/random_util.hpp \
	../include/search.hpp \
	../include/search_temper.hpp \
	../include/shortest_basis.hpp \
	mtgp32.hpp \
	mtgp64.hpp \
	output.hpp \

all: mtgp32dc mtgp64dc

mtgp32dc: mtgp32dc.cpp parse_opt.o ${HEADERS}
	${CPP} -o $@ mtgp32dc.cpp parse_opt.o ${LINKOPT}

mtgp32dc-check: mtgp32dc-check.cpp ${HEADERS}
	${CPP} -o $@ mtgp32dc-check.cpp ${LINKOPT}

mtgp64dc: mtgp64dc.cpp parse_opt.o ${HEADERS}
	${CPP} -o $@ mtgp64dc.cpp parse_opt.o ${LINKOPT}

.cpp.o:
	${CPP} -c $<

clean:
	rm -f *.o *~
