################################## DEPENDENCIES ###################################

#
# Boost (version 1.40.0)
#
SET( Boost_USE_STATIC_LIBS   ON )
SET( Boost_USE_MULTITHREADED ON )
SET( Boost_ADDITIONAL_VERSIONS "1.40" "1.40.0" )
SET( BOOST_ROOT ${VCT_build_SOURCE_DIR}/vct/src/Dependencies/boost )
MESSAGE( "\nINFO: Looking for Boost @ ${BOOST_ROOT}" )
FIND_PACKAGE( Boost 1.40.0
	REQUIRED
	filesystem
	system
	regex
	date_time
	thread
)
IF( Boost_FOUND )
	MESSAGE( "... success!" )
	MESSAGE( "    Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}" )
	MESSAGE( "    Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}" )
	MESSAGE( "    Boost_LIBRARIES:    ${Boost_LIBRARIES}" )
ELSE( Boost_FOUND )
	MESSAGE( "... fail!" )
	RETURN()
ENDIF( Boost_FOUND )


#
# OpenCV
#
# Windows specific
SET( OpenCV_ROOT_DIR ${VCT_build_SOURCE_DIR}/vct/src/Dependencies/OpenCV )
SET( OpenCV_INCLUDE_DIRS
	${OpenCV_ROOT_DIR}/cv/include
	${OpenCV_ROOT_DIR}/cxcore/include
	${OpenCV_ROOT_DIR}/cvaux/include
	${OpenCV_ROOT_DIR}/otherlibs/highgui
)
SET( OpenCV_LINK_DIRECTORIES
	${OpenCV_ROOT_DIR}/lib
)
SET( OpenCV_LIBRARIES
	cv.lib
	cvaux.lib
	cxcore.lib
	highgui.lib
)
SET( OpenCV_FOUND NOT NOTFOUND )

MESSAGE( "\nINFO: Looking for OpenCV @ ${OpenCV_ROOT_DIR}" )
IF( OpenCV_FOUND )
	MESSAGE( "... success!" )
	MESSAGE( "    OpenCV_INCLUDE_DIRS:     ${OpenCV_INCLUDE_DIRS}" )
	MESSAGE( "    OpenCV_LINK_DIRECTORIES: ${OpenCV_LINK_DIRECTORIES}" )
	MESSAGE( "    OpenCV_LIBRARIES:        ${OpenCV_LIBRARIES}" )
ELSE( OpenCV_FOUND )
	MESSAGE( "... fail!" )
	RETURN()
ENDIF( OpenCV_FOUND )


#
# Meschach (Windows only for now)
# 
# Windows specific
	SET( Meschach_ROOT_DIR ${VCT_build_SOURCE_DIR}/vct/src/Modules/HotellingObserverModule/Lib_Meschach )
	SET( Meschach_FOUND NOT NOTFOUND )
	SET( Meschach_INCLUDE_DIR
		${Meschach_ROOT_DIR}
	)
	SET( Meschach_LINK_DIR ${Meschach_ROOT_DIR}/$(ConfigurationName) )
	SET( Meschach_LIBRARIES
		Meschach_MDd.lib
	)
MESSAGE( "\nINFO: Looking for Meschach @ ${Meschach_ROOT_DIR}" )
IF( Meschach_FOUND )
	MESSAGE( "... success!" )
	MESSAGE( "    Meschach_INCLUDE_DIR: ${Meschach_INCLUDE_DIR}" )
	MESSAGE( "    Meschach_LINK_DIR:    ${Meschach_LINK_DIR}" )
	MESSAGE( "    Meschach_LIBRARIES:   ${Meschach_LIBRARIES}" )
ELSE( Meschach_FOUND )
	MESSAGE( "... fail!" )
	MESSAGE( "Meschach libraries are not available for this build" )
	RETURN()
ENDIF( Meschach_FOUND )

#
# OpenCL (optional, meaning build succeeds without, but then you cannot use VCT's OCL modules)
# You need from _any_ single vendor: CL/cl.hpp and OpenCL.lib/libOpenCL.a
# And then the registered OpenCL ICD(s) (installable client driver(s)) from the vendor(s) you want to use.
# On Windows ICD registration is automatic.
# On Linux each ICD must be represented in /etc/OpenCL/vendors/

# SET(VCT_USE_OPENCL
# TRUE
# )
IF(VCT_USE_OPENCL)
	# Looking for OpenCL
	set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
	message ("CMAKE_MODULE_PATH:" ${CMAKE_MODULE_PATH})
	find_package( OpenCL REQUIRED )
# Commented: the code below does not work for all the configurations
	# Windows specific
	SET( OpenCL_ROOT_DIR "C:/Program Files/AMD APP" )
	SET( OpenCL_INCLUDE_DIR ${OpenCL_ROOT_DIR}/include )
	SET( OpenCL_LINK_DIR ${OpenCL_ROOT_DIR}/lib/x86 )
	SET( OpenCL_LIBRARIES OpenCL.lib )
		SET( OpenCL_FOUND NOT NOTFOUND )
	MESSAGE( "\nINFO: Looking for OpenCL @ ${OpenCL_ROOT_DIR}" )
	IF( OPENCL_FOUND )
		MESSAGE( "... success!" )
		MESSAGE( "    OpenCL_INCLUDE_DIR: ${OPENCL_INCLUDE_DIRS}" )
		#MESSAGE( "    OpenCL_LINK_DIR:    ${OPENCL_LINK_DIR}" )
		MESSAGE( "    OpenCL_LIBRARIES:   ${OPENCL_LIBRARIES}" )
	ELSE( OPENCL_FOUND )
		MESSAGE( "... fail! Continuing, but in this build you cannot use OpenCL modules with VCT!" )
	ENDIF( OPENCL_FOUND )
ENDIF()

############################# INCLUDE DIRECTORIES #############################
INCLUDE_DIRECTORIES(
	${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	${OPENCL_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container
	${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies
)

############################## LINK DIRECTORIES ###############################
LINK_DIRECTORIES (
	${Boost_LIBRARY_DIRS}
	${OpenCV_LINK_DIRECTORIES}
	${OPENCL_LINK_DIR}
)

SET( dependencieslibs
	${Boost_LIBRARIES}
	${OpenCV_LIBRARIES}
	${OPENCL_LIBRARIES}
	)
#################################### BUILD #####################################
# OpenCL host source files to compile (OpenCL kernel files are added below at POST-BUILD).
IF( OPENCL_FOUND )
	ADD_DEFINITIONS( -DUSE_OPENCL=1 )	# for the (few) location(s) in always built sources that refer(s) to files using OpenCL
		SET ( OPENCL_KERNEL_SUBDIR
		opencl-kernels		# no trailing '/'
	)
	ADD_DEFINITIONS( -DOPENCL_KERNEL_SUBDIR=${OPENCL_KERNEL_SUBDIR} )

	# 2012/12/19 : if the following modules are added, VCT does not compile anymore.
	# To be checked why
	SET ( OPENCL_HOST_SOURCES
		# VCT core OpenCL sources
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/OpenCLManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/OpenCLErrorStrings.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies/rng/mtgp-opencl/mtgp32dc-param-11213.cpp		# doesn't contain OpenCL refs, but only needed for OpenCL RNG


	)
ENDIF( OPENCL_FOUND )
# OpenCL kernel sources and headers that must be copied to the final binary directory.
IF( OPENCL_FOUND )
	SET ( OPENCL_KERNEL_SUBDIR
		opencl-kernels		# no trailing '/'
	)
	ADD_DEFINITIONS( -DOPENCL_KERNEL_SUBDIR=${OPENCL_KERNEL_SUBDIR} )

	SET ( OPENCL_KERNEL_DIR
		${FINAL_BIN_DIR}/${OPENCL_KERNEL_SUBDIR}
	)

	#GUSP commented
	# ATM this only works for Linux, so also add kernel sources and header copy commands below under WIN32
	SET ( OPENCL_KERNEL_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies/rng/mtgp-opencl/mtgp32-ocl-kernels.cl
	)
	SET ( OPENCL_KERNEL_HEADERS
		${CMAKE_CURRENT_SOURCE_DIR}/src/Container/opencl-kernels-common.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies/rng/mtgp-opencl/mtgp32-ocl-kernels.h
	)
ENDIF( OPENCL_FOUND )

SET(dependencieslist
	lut
	Meschach
	zlib
	utilities
	other
	HotellingObserver
)
#defines the sources of the dependencies
SET(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies)

SET(lutdependencies
	${DEPENDENCIES_DIR}/lut/LookUpTable.cpp
)

SET(Meschachdependencies
	${DEPENDENCIES_DIR}/Meschach/arnoldi.c
	${DEPENDENCIES_DIR}/Meschach/bdfactor.c
	${DEPENDENCIES_DIR}/Meschach/bkpfacto.c
	${DEPENDENCIES_DIR}/Meschach/chfactor.c
	${DEPENDENCIES_DIR}/Meschach/conjgrad.c
	${DEPENDENCIES_DIR}/Meschach/copy.c
	${DEPENDENCIES_DIR}/Meschach/err.c
	${DEPENDENCIES_DIR}/Meschach/err.h
	${DEPENDENCIES_DIR}/Meschach/extras.c
	${DEPENDENCIES_DIR}/Meschach/fft.c
	${DEPENDENCIES_DIR}/Meschach/givens.c
	${DEPENDENCIES_DIR}/Meschach/hessen.c
	${DEPENDENCIES_DIR}/Meschach/hsehldr.c
	${DEPENDENCIES_DIR}/Meschach/init.c
	${DEPENDENCIES_DIR}/Meschach/iter.h
	${DEPENDENCIES_DIR}/Meschach/iter0.c
	${DEPENDENCIES_DIR}/Meschach/iternsym.c
	${DEPENDENCIES_DIR}/Meschach/itersym.c
	${DEPENDENCIES_DIR}/Meschach/ivecop.c
	${DEPENDENCIES_DIR}/Meschach/lanczos.c
	${DEPENDENCIES_DIR}/Meschach/lufactor.c
	${DEPENDENCIES_DIR}/Meschach/machine.c
	${DEPENDENCIES_DIR}/Meschach/machine.h
	${DEPENDENCIES_DIR}/Meschach/matlab.c
	${DEPENDENCIES_DIR}/Meschach/matlab.h
	${DEPENDENCIES_DIR}/Meschach/matop.c
	${DEPENDENCIES_DIR}/Meschach/matrix.h
	${DEPENDENCIES_DIR}/Meschach/matrix2.h
	${DEPENDENCIES_DIR}/Meschach/matrixio.c
	${DEPENDENCIES_DIR}/Meschach/meminfo.c
	${DEPENDENCIES_DIR}/Meschach/meminfo.h
	${DEPENDENCIES_DIR}/Meschach/memory.c
	${DEPENDENCIES_DIR}/Meschach/memstat.c
	${DEPENDENCIES_DIR}/Meschach/mfunc.c
	${DEPENDENCIES_DIR}/Meschach/mmatrix.h
	${DEPENDENCIES_DIR}/Meschach/norm.c
	${DEPENDENCIES_DIR}/Meschach/oldnames.h
	${DEPENDENCIES_DIR}/Meschach/otherio.c
	${DEPENDENCIES_DIR}/Meschach/pxop.c
	${DEPENDENCIES_DIR}/Meschach/qrfactor.c
	${DEPENDENCIES_DIR}/Meschach/schur.c
	${DEPENDENCIES_DIR}/Meschach/solve.c
	${DEPENDENCIES_DIR}/Meschach/sparse.c
	${DEPENDENCIES_DIR}/Meschach/sparse.h
	${DEPENDENCIES_DIR}/Meschach/sparse2.h
	${DEPENDENCIES_DIR}/Meschach/sparseio.c
	${DEPENDENCIES_DIR}/Meschach/spbkp.c
	${DEPENDENCIES_DIR}/Meschach/spchfctr.c
	${DEPENDENCIES_DIR}/Meschach/splufctr.c
	${DEPENDENCIES_DIR}/Meschach/sprow.c
	${DEPENDENCIES_DIR}/Meschach/spswap.c
	${DEPENDENCIES_DIR}/Meschach/submat.c
	${DEPENDENCIES_DIR}/Meschach/svd.c
	${DEPENDENCIES_DIR}/Meschach/symmeig.c
	${DEPENDENCIES_DIR}/Meschach/update.c
	${DEPENDENCIES_DIR}/Meschach/vecop.c
	${DEPENDENCIES_DIR}/Meschach/version.c
	${DEPENDENCIES_DIR}/Meschach/zcopy.c
	${DEPENDENCIES_DIR}/Meschach/zfunc.c
	${DEPENDENCIES_DIR}/Meschach/zgivens.c
	${DEPENDENCIES_DIR}/Meschach/zhessen.c
	${DEPENDENCIES_DIR}/Meschach/zhsehldr.c
	${DEPENDENCIES_DIR}/Meschach/zlufctr.c
	${DEPENDENCIES_DIR}/Meschach/zmachine.c
	${DEPENDENCIES_DIR}/Meschach/zmatio.c
	${DEPENDENCIES_DIR}/Meschach/zmatlab.c
	${DEPENDENCIES_DIR}/Meschach/zmatop.c
	${DEPENDENCIES_DIR}/Meschach/zmatrix.h
	${DEPENDENCIES_DIR}/Meschach/zmatrix2.h
	${DEPENDENCIES_DIR}/Meschach/zmemory.c
	${DEPENDENCIES_DIR}/Meschach/znorm.c
	${DEPENDENCIES_DIR}/Meschach/zqrfctr.c
	${DEPENDENCIES_DIR}/Meschach/zschur.c
	${DEPENDENCIES_DIR}/Meschach/zsolve.c
	${DEPENDENCIES_DIR}/Meschach/zvecop.c
)

SET(HotellingObserverdependencies
	${DEPENDENCIES_DIR}/HotellingObserver/channel_laguerre.h
	${DEPENDENCIES_DIR}/HotellingObserver/ChannelFilter.h
	${DEPENDENCIES_DIR}/HotellingObserver/iq.h
	${DEPENDENCIES_DIR}/HotellingObserver/channel_laguerre1d.c
	${DEPENDENCIES_DIR}/HotellingObserver/channel_laguerre2d.c
	${DEPENDENCIES_DIR}/HotellingObserver/channel_laguerre3d.cpp
	${DEPENDENCIES_DIR}/HotellingObserver/ChannelFilter.cpp
	${DEPENDENCIES_DIR}/HotellingObserver/chotelling.c
	${DEPENDENCIES_DIR}/HotellingObserver/dprime.cpp
	${DEPENDENCIES_DIR}/HotellingObserver/symeig.c
	${DEPENDENCIES_DIR}/HotellingObserver/wilcoxonauc.c
)
SET(zlibdependencies
	${DEPENDENCIES_DIR}/zlib/adler32.c
	${DEPENDENCIES_DIR}/zlib/compress.c
	${DEPENDENCIES_DIR}/zlib/crc32.c
	${DEPENDENCIES_DIR}/zlib/deflate.c
	${DEPENDENCIES_DIR}/zlib/gzio.c
	${DEPENDENCIES_DIR}/zlib/infback.c
	${DEPENDENCIES_DIR}/zlib/inffast.c
	${DEPENDENCIES_DIR}/zlib/inflate.c
	${DEPENDENCIES_DIR}/zlib/inftrees.c
	${DEPENDENCIES_DIR}/zlib/trees.c
	${DEPENDENCIES_DIR}/zlib/uncompr.c
	${DEPENDENCIES_DIR}/zlib/zutil.c
)
SET(utilitiesdependencies
	${DEPENDENCIES_DIR}/utilities/UtilitiesDisplay.cpp
	${DEPENDENCIES_DIR}/utilities/UtilitiesMeasProc.cpp
	${DEPENDENCIES_DIR}/utilities/DataStore.cpp
)

# Dependencies files
SET (otherdependencies
# # Dependencies
		# # color
	${DEPENDENCIES_DIR}/color/color.cpp
		# # FileFinder
	${DEPENDENCIES_DIR}/FileFinder/FileFinder.cpp
		# # i_o bmp
	${DEPENDENCIES_DIR}/i_o/bmp/EasyBMP.cpp
		# # iConvert
	${DEPENDENCIES_DIR}/iConvert/iConvert.cpp
		# # Meshgrid
	${DEPENDENCIES_DIR}/Meshgrid/Meshgrid.cpp
		# # misc
	${DEPENDENCIES_DIR}/misc/misc.cpp
		# # xml
	${DEPENDENCIES_DIR}/xml/xmlParser.cpp	
	
	${OPENCL_KERNEL_HEADERS}
	${OPENCL_KERNEL_SOURCES}
	${OPENCL_HOST_SOURCES}
)

#Add all the sources of the dependencies in one variable
foreach(dependency ${dependencieslist})
SET(FILES_dependency "${${dependency}dependencies}")
SET(dependencies
	${dependencies}
	${FILES_dependency}
)
endforeach(dependency)