# Enable the solution folders
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# Configuration types
SET(CMAKE_CONFIGURATION_TYPES Debug Release)
SET(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)

# CMake minimum version required
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.10.2 FATAL_ERROR )

# Project
PROJECT( VCT_build)

# Preprocessor definitions
IF( WIN32 ) # Windows specific
	ADD_DEFINITIONS( -DWIN32 )
ENDIF( WIN32 )

# Compiler flags
IF( ${CMAKE_BUILD_TYPE} MATCHES Release ) # Release build
	ADD_DEFINITIONS( -DNDEBUG=1 )
ELSEIF( ${CMAKE_BUILD_TYPE} MATCHES Debug ) # Debug build
	ADD_DEFINITIONS( -D_DEBUG=1 )
	
ENDIF()

# Final binaries directory
SET( FINAL_BIN_DIR ${VCT_build_BINARY_DIR}/${CMAKE_BUILD_TYPE} )
FILE( MAKE_DIRECTORY ${FINAL_BIN_DIR} )
MESSAGE( "\nINFO: Final binaries will be located @\n${FINAL_BIN_DIR}" )
include (${CMAKE_CURRENT_SOURCE_DIR}/Common_Macros.cmake)

SET(CMAKE_CURRENT_SOURCE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/..
)
SET(VCT_build_SOURCE_DIR
	${VCT_build_SOURCE_DIR}/../..
)
################################# PREPROCESSOR #################################
IF( WIN32 ) # Windows specific
	ADD_DEFINITIONS( -DWIN32 )
	ADD_DEFINITIONS( -D_SCL_SECURE_NO_WARNINGS )
ENDIF( WIN32 )
# Definition for Meschach
ADD_DEFINITIONS ( -DSPARSE )

ADD_DEFINITIONS ( -DCOMPLEX )

ADD_DEFINITIONS( -D_UNICODE ) # XML Unicode support

ADD_DEFINITIONS( -DBoost_POSIX_API ) # Boost

SET(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:LIBCMT) #aympic 04/04/2013 remove the LIBCMT library by default -> because LIBCMT created conflict between dlls

################################## PRE-BUILD ###################################


#If the administrator wants to exclude some modules
SET(notcompiledmodules
	
)


#include the file of the group's configurations
include (${CMAKE_CURRENT_SOURCE_DIR}/build_${COMPILATION_TYPE}/ModulesList.cmake)

##### Check if the modules in ${defaultmodules} and the personnal modules (listed in manual_conf.txt) are public, restricted, or unusable
##### (See Common_Macros.cmake)
Find_Modules()

##### suppress the redundancy in the module list: ${allmodules}
##### (See Common_Macros.cmake)
suppress_redundancy(allmodules)

suppress_redundancy(Libslist)

# prepare the list of the file of the modules from the list of the names of the modules.
foreach(module ${publicmodules})
	include (${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/cpp/BuildFull.cmake)
	# message("##################### ${module} ###########################")
	SET(FILES_NAME "${FILES_MODULES_${module}}")
	# message(${FILES_NAME}	)
	SET ( MODULES
		${MODULES}
		${FILES_NAME}
	)
endforeach(module) 

##### prepare the list of the modules' libraries to be linked to the project
##### (See Common_Macros.cmake)
LinkLibraries()

# include the management of the dependencies: description to the libraries or link to the libs
include (${CMAKE_CURRENT_SOURCE_DIR}/build_${COMPILATION_TYPE}/Dependencies.cmake)

##### Exclude from the compilation the files specified by the administrator
##### (See Common_Macros.cmake)
Exclude_Files()


#prepare the list of the compiled modules for the FactoryModule generation.
foreach(module ${allmodules})
	GET_SOURCE_FILE_PROPERTY(is_ignored ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/${module}.h HEADER_FILE_ONLY)
	IF(${is_ignored} MATCHES "NOTFOUND" OR ${is_ignored} MATCHES "FALSE")
	SET(compiledmodules
		${compiledmodules}
		${module}
	)
	ENDIF()
endforeach(module)

##### Generate the file FactoryModule.cpp from the list of the activated modules
##### (See Common_Macros.cmake)
Factory_Generator()


##### Write the list of the compiled modules in a text file to be compared with the list of the test unit, in order to avoid to run a test unit of a module that is not currently on VCT
##### (See Common_Macros.cmake)
Generate_list_of_modules_for_test_units()

# Core files
SET ( FILES_CORE
	# Container
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/Channel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/DSContainer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/FactoryModule.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/Module.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/TypeTraits.cpp
		# DataExchanger
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/DataExchanger/UtilitiesExchanger.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/DataExchanger/UtilitiesInterface.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/DataExchanger/ContainerExchanger.cpp
		# ErrorManagement
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/ErrorManagement/ErrorLogger.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/ErrorManagement/MevicLogger.cpp
		# SuperXML
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/AbstractSimulation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/FactoryParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/InOutManagement.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/ParamBasic.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/ParamIt.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/ParamItFormatted.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/ParamStar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/ParamStep.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/ParamString.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/Parallel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/Simulation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/Chain.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/Pipeline.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/Iterations.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Container/SuperXML/SuperMEVICXML.cpp
)

# main
SET ( FILE_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp )

##### Make the project to compile the libraries of the modules specified in the variable ${Libslist}
##### (See Common_Macros.cmake)
Compile_Libraries()
# Build target VCT
ADD_EXECUTABLE( VCT 
	${FILE_MAIN}
	${FILES_CORE}
	${dependencies}	
	${MODULES}
 )

# Organization in folders 
foreach(dependency ${dependencieslist})
	SET(FILES_dependency "${${dependency}dependencies}")
	source_group ("Dependencies\\${dependency}" FILES	${FILES_dependency})
endforeach(dependency)

source_group ("" FILES ${FILE_MAIN}	)
source_group (Container FILES ${FILES_CORE}	)

foreach(module ${publicmodules})
	SET(HEADER_NAME "${HEADER_${module}}")
	SET(SRC_NAME "${SRC_${module}}")
	source_group ("Modules\\${module}" FILES ${HEADER_NAME}	)
	source_group ("Modules\\${module}\\cpp" FILES ${SRC_NAME}	)
endforeach(module)

private_function()
#add_library(test_cu $<TARGET_OBJECTS:objSource>)
# Link target against Libraries
TARGET_LINK_LIBRARIES (
	VCT
	${TARGET_LNK_LIBRARIES}
	${dependencieslibs}
	${PRIVATE_LIBRARIES}
)

################################## POST-BUILD ##################################


# Copy the target to ${FINAL_BIN_DIR}
IF( WIN32 ) # Windows specific
	ADD_CUSTOM_COMMAND(
		TARGET     VCT
		POST_BUILD
		COMMAND    copy /Y ..\\vct\\src\\Dependencies\\OpenCV\\bin\\cv110.dll .\\$(ConfigurationName)
		COMMAND    copy /Y ..\\vct\\src\\Dependencies\\OpenCV\\bin\\cvaux110.dll .\\$(ConfigurationName)
		COMMAND    copy /Y ..\\vct\\src\\Dependencies\\OpenCV\\bin\\cxcore110.dll .\\$(ConfigurationName)
		COMMAND    copy /Y ..\\vct\\src\\Dependencies\\OpenCV\\bin\\highgui110.dll .\\$(ConfigurationName)
		COMMAND    if not exist .\\$(ConfigurationName)\\${OPENCL_KERNEL_SUBDIR} mkdir .\\$(ConfigurationName)\\${OPENCL_KERNEL_SUBDIR}
		COMMAND    copy /Y ..\\vct\\src\\Container\\opencl-kernels-common.h .\\$(ConfigurationName)\\${OPENCL_KERNEL_SUBDIR}
		COMMAND    copy /Y ..\\vct\\src\\Dependencies\\rng\\mtgp-opencl\\mtgp32-ocl-kernels.cl .\\$(ConfigurationName)\\${OPENCL_KERNEL_SUBDIR}
		COMMAND    copy /Y ..\\vct\\src\\Dependencies\\rng\\mtgp-opencl\\mtgp32-ocl-kernels.h .\\$(ConfigurationName)\\${OPENCL_KERNEL_SUBDIR}
		COMMENT    "+++ Copying VCT, libraries and OpenCL kernel sources to Final binaries directory ($(ConfigurationName)) +++"
		VERBATIM
	)

IF ( OPENCL_FOUND)
	ADD_CUSTOM_COMMAND(
	TARGET     VCT
	POST_BUILD
	COMMAND    copy /Y ${OPENCL_DLL_DIR}\\${OPENCL_DLL} .\\$(ConfigurationName)
	COMMENT    "+++ Copying OpenCL dll ($(ConfigurationName)) +++"
	VERBATIM
	)
ENDIF ()
	
ENDIF()