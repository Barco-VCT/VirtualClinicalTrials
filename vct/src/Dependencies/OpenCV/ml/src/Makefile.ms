TARGET = ml
BINTYPE = DLL
ROOT = ..\..
PCH = _ml.h
PCH_STARTER = ml

OBJS = $(OBJPATH)/mlann_mlp.obj \
    $(OBJPATH)/mlboost.obj $(OBJPATH)/mlcnn.obj $(OBJPATH)/mlem.obj \
    $(OBJPATH)/mlestimate.obj $(OBJPATH)/mlknearest.obj $(OBJPATH)/mlnbayes.obj \
    $(OBJPATH)/mlrtrees.obj $(OBJPATH)/mlsvm.obj $(OBJPATH)/mltestset.obj \
    $(OBJPATH)/mltree.obj $(OBJPATH)/ml_inner_functions.obj   

INCS = ../include/ml.h ./_ml.h \
      $(ROOT)/cxcore/include/cxcore.h $(ROOT)/cxcore/include/cxcore.hpp \
      $(ROOT)/cxcore/include/cxerror.h $(ROOT)/cxcore/include/cxmisc.h \
      $(ROOT)/cxcore/include/cxtypes.h $(ROOT)/cxcore/include/cvver.h

CXXFLAGS_PROJ = /I"." /I"../include" /I"$(ROOT)/cxcore/include"
LIBS_PROJ = $(LIBPATH)"$(ROOT)/lib" cxcore$(DBG)$(OUT_P_SUFFIX).lib

!include $(ROOT)/_make\make_module_$(MS).mak
