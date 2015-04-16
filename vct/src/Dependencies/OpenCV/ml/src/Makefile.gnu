TARGET := ml
BINTYPE := DLL
SRC_ROOT := ../../ml/src
INC_ROOT := ../../ml/include
CXCORE_INC := ../../cxcore/include
SRC_DIRS := . ../include ../../cxcore/include

CXXFLAGS := -D"CVAPI_EXPORTS" -I"$(INC_ROOT)" -I"$(SRC_ROOT)" -I"$(CXCORE_INC)"

INCS := ml.h _ml.h \
   cxcore.h cxcore.hpp cxerror.h cxmisc.h cxtypes.h cvver.h

LIBS := -lcxcore$(DBG)

include ../../_make/make_module_gnu.mak
