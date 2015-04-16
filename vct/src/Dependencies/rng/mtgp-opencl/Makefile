# Makefile for Mersenne Twister for Graphics Processors (MTGP), retargeted from CUDA to OpenCL.
# Original sources from http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MTGP/index.html
CC			= gcc
CPP			= g++
CLC			= openclcc
#CPPFLAGS		= -msse3 -O3 -ffast-math -Wall -DNDEBUG
CPPFLAGS		= -g3 -Wall

SYS64			= $(shell uname -m | grep 64)
ifeq "$(strip $(SYS64))" ""
BITS			= 32
ARCH			= x86
else
BITS			= 64
ARCH			= x86_64
endif

HOSTNAME                = $(shell hostname)
ifeq ($(HOSTNAME),kndclt21574.barco.com)
OCLROOT			= /home/aleva/usr/ati-stream-sdk-v2.3-lnx$(BITS)
#else ifeq ($(HOSTNAME),another.hostname.org)
else
OCLROOT			= /usr/local/ati-stream-sdk-v2.3-lnx$(BITS)
endif

INC			= $(OCLROOT)/include
LDFLAGS			= $(OCLROOT)/lib/$(ARCH)

SRC32		= mtgp32-ocl.cpp mtgp32-ocl-common.cpp mtgp32-fast.c mtgp32dc-param-11213.c
HDR32		= mtgp32-ocl-common.h
SRC64		= mtgp64-ocl.cpp mtgp64-ocl-common.cpp mtgp64-fast.c mtgp64dc-param-11213.c
HDR64		= mtgp64-ocl-common.h
SRCCOMMON	= mtgp-ocl-common.cpp
HDRCOMMON	= mtgp-ocl-common.h
# OpenCL sources must be listed in order of dep satisfaction (i.e. if x.cl depends on x.h: x.h x.cl)
CL32		= mtgp32-ocl-kernels.cl
CL64		= mtgp64-ocl-kernels.cl


#all: mtgp32-ocl mtgp64-ocl


mtgp32-ocl: ${SRC32} ${HDR32} ${SRCCOMMON} ${HDRCOMMON}
	${CPP} ${CPPFLAGS} -I${INC} -L${LDFLAGS} -o mtgp32-ocl ${SRC32} ${SRCCOMMON} -lOpenCL

mtgp64-ocl: ${SRC64} ${HDR64} ${SRCCOMMON} ${HDRCOMMON}
	${CPP} ${CPPFLAGS} -I${INC} -L${LDFLAGS} -o mtgp64-ocl ${SRC64} ${SRCCOMMON} -lOpenCL


# To test-compile the .cl file(s). See http://code.google.com/p/openclcc/
cl32: ${CL32}
	${CLC} ${CL32}
	
cl64: ${CL64}
	${CLC} ${CL64}


clean:
	rm -rf mtgp32-ocl mtgp64-ocl

