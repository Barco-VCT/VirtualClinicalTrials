###### This file contains some private configuration of VCT
SET(defaultmodules
Apply3x1DLutModule
ConversionDDL2CDModule
ConvertRawToChoBinModule
DeltaE2000Module
DisplayLutModule
DisplayModule
MRMCModule
ReaderModule
Rgb2XYZDisplayModule
SaveFrameBMPModule
SaveFrameRAWModule
SaveFrameTXTModule
SequenceRawGeneratorModule
SingleSliceCHOModule
SRgbDisplayModule
VideoCardModule
WriterModule
)

#public modules the user wants to see in the solution, but not compile
SET(notcompiledmodules
	${notcompiledmodules}
)
SET(PRIVATE_LIBRARIES

)
###here add your private Cmake code


function(private_function)
#here add the private code you want to execute after the creation of the Target (after the add executable)

endfunction()

