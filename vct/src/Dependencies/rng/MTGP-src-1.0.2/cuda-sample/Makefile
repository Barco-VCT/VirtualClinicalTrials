# @file  Makefile
# @brief Makefile
#
# @author Mutsuo Saito (Hiroshima University)
# @author Makoto Matsumoto (Hiroshima University)
#
# Copyright (C) 2009, 2010 Mutsuo Saito, Makoto Matsumoto and
# Hiroshima University. All rights reserved.
#
# The new BSD License is applied to this software.
# see LICENSE.txt
#

#DEBUG = -DDEBUG -g -O0
WARN = -Wmissing-prototypes -Wall #-Winline
OPTI = -O3 -finline-functions -fomit-frame-pointer -DNDEBUG \
-fno-strict-aliasing

STD = -std=c99
CC = gcc
#ARCH = -arch=compute_11
ARCH = -arch=compute_13
NVCC = nvcc -DDEBUG $(ARCH) -ccbin=/tmp #--compiler-options '-Wall'

# for OS X
#CUTILINC = -I"/Developer/GPU Computing/C/common/inc" -I../
#CUTILLIB = -L"/Developer/GPU Computing/C/lib" -lcutil_i386
# for linux
CUTILINC = -I$(HOME)/NVIDIA_GPU_Computing_SDK/C/common/inc -I../
CUTILLIB = -L$(HOME)/NVIDIA_GPU_Computing_SDK/C/lib -lcutil

CUDALINK = -lcuda
CPP = g++
CPPFLAGS = -Wall -Wextra -O3
CCFLAGS = $(OPTI) $(WARN) $(STD) $(DEBUG)

.SUFFIXES:	.cu .cubin .ptx .bin

samples:sample32-cuda sample32-cuda-tex sample64-cuda sample64-cuda-tex \
	sample64-cuda13-tex

sample32-cuda: mtgp32-cuda.cu ../mtgp32-fast.h mtgp32dc-param-11213.c \
	../mtgp32-fast.o mtgp32-cuda-common.c mtgp-cuda-common.c
	${NVCC} -o $@ mtgp32-cuda.cu ../mtgp32-fast.o \
	 ${CUTILINC} ${CUTILLIB} ${CUDALINK}

sample32-cuda-tex: mtgp32-cuda-tex.cu ../mtgp32-fast.h mtgp32dc-param-11213.c \
	../mtgp32-fast.o mtgp32-cuda-common.c mtgp-cuda-common.c
	${NVCC} -o $@ mtgp32-cuda-tex.cu ../mtgp32-fast.o \
	 ${CUTILINC} ${CUTILLIB} ${CUDALINK}

sample32-cuda23209-tex: mtgp32-cuda23209-tex.cu ../mtgp32-fast.h \
	../mtgp32-param-fast.c \
	../mtgp32-fast.o mtgp32-cuda-common.c mtgp-cuda-common.c
	${NVCC} -o $@ mtgp32-cuda-tex.cu ../mtgp32-fast.o \
	../mtgp32-param-fast.o \
	 ${CUTILINC} ${CUTILLIB} ${CUDALINK}

sample64-cuda: mtgp64-cuda.cu ../mtgp64-fast.h mtgp64dc-param-11213.c \
	../mtgp64-fast.o mtgp64-cuda-common.c mtgp-cuda-common.c
	${NVCC} -o $@ mtgp64-cuda.cu ../mtgp64-fast.o \
	 ${CUTILINC} ${CUTILLIB} ${CUDALINK}

sample64-cuda-tex: mtgp64-cuda-tex.cu ../mtgp64-fast.h mtgp64dc-param-11213.c \
	../mtgp64-fast.o mtgp64-cuda-common.c mtgp-cuda-common.c
	${NVCC} -o $@ mtgp64-cuda-tex.cu ../mtgp64-fast.o \
	 ${CUTILINC} ${CUTILLIB} ${CUDALINK}

sample64-cuda-tex-small: mtgp64-cuda-tex-small.cu \
	../mtgp64-fast.h mtgp64dc-param-11213.c \
	../mtgp64-fast.o mtgp64-cuda-common.c mtgp-cuda-common.c
	${NVCC} -o $@ mtgp64-cuda-tex-small.cu ../mtgp64-fast.o \
	 ${CUTILINC} ${CUTILLIB} ${CUDALINK}

sample64-cuda13-tex: mtgp64-cuda13-tex.cu \
	../mtgp64-fast.h mtgp64dc-param-11213.c \
	../mtgp64-fast.o mtgp64-cuda-common.c mtgp-cuda-common.c
	${NVCC} -arch=compute_13 -o $@ mtgp64-cuda13-tex.cu ../mtgp64-fast.o \
	 ${CUTILINC} ${CUTILLIB} ${CUDALINK}

mt32-cuda-tex: mt32-cuda-tex.cu mt32dc-params521.c \
	mtgp32-cuda-common.c mtgp-cuda-common.c
	${NVCC} ${ARCH} -I./ -o $@ mt32-cuda-tex.cu \
	 ${CUTILINC} ${CUTILLIB} ${CUDALINK}

mt32-cuda-tex-reg.bin: mt32-cuda-tex-reg.cu mt32dc-params521.c \
	mtgp32-cuda-common.c mtgp-cuda-common.c
	${NVCC} -I./ --maxrregcount=32 -o $@ mt32-cuda-tex-reg.cu \
	 ${CUTILINC} ${CUTILLIB} ${CUDALINK}

.cu.ptx:
	${NVCC} ${CUTILINC} -ptx $<

.cu.cubin:
	${NVCC} ${CUTILINC} -cubin $<

.cu.bin:
	${NVCC} ${ARCH} ${CUTILINC} -o $@ $< ${CUTILLIB} ${CUDALINK}

.c.o:
	${CC} ${CCFLAGS} -c $<

clean:
	rm -f *.o *~
