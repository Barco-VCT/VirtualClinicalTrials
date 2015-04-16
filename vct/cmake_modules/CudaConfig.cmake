# CMake minimum version required
################################## FindCuda ###################################

#Find CUDA
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/cuda" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(CUDA)
if (${CUDA_FOUND})
    message(STATUS "Found CUDA")



if(CUDA_FOUND)
set(CMAKE_CUDA_SOURCE_FILE_EXTENSIONS cu)
set(CMAKE_CUDA_IGNORE_EXTENSIONS h;H;o;O;obj;OBJ;def;DEF;rc;RC)

set(CMAKE_CUDA_COMPILE_OBJECT 
  "<CMAKE_CUDA_COMPILER> <DEFINES> -c <SOURCE> -o <OBJECT> -Xcompiler <FLAGS>"
  )

set(CMAKE_CUDA_OUTPUT_EXTENSION ${CMAKE_CXX_OUTPUT_EXTENSION})
set(CMAKE_CUDA_COMPILER_ENV_VAR "NVCC")
	#enable_language(CUDA)
    SET(CUDA_NVCC_FLAGS "-arch;sm_20")
else(CUDA_FOUND)
    message("CUDA is not installed on this system.")
endif()

#Find the SDK path in for cutil includes
find_path(CUDA_SDK_ROOT_DIR common/inc/cutil.h
  "$ENV{HOME}/NVIDIA_GPU_Computing_SDK/C"
  "$ENV{NVSDKCOMPUTE_ROOT}/C"
  "$ENV{NVSDKCUDA_ROOT}"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\NVIDIA Corporation\\Installed Products\\NVIDIA SDK 10\\Compute;InstallDir]"
  "/Developer/GPU\ Computing/C"
)

############################# INCLUDE DIRECTORIES AND LINK DIRECTORIES #############################

	INCLUDE_DIRECTORIES(
		${CUDA_INCLUDE_DIRS}
		${CUDA_SDK_ROOT_DIR}/common/inc
	)
	
	LINK_DIRECTORIES (
		${CUDA_INCLUDE_DIRS}/../lib/Win32
		${CUDA_SDK_ROOT_DIR}/common/lib/Win32
	)

############################# SET  LIBRARIES TO BE ADDED INTO TARGET_LINK_LIBRARIES #############################

	SET( CUDA_ADD_LIBRARIES
		cudart.lib
		cuda.lib
		debug cutil32D.lib
		optimized cutil32.lib
	)
else (${CUDA_FOUND})
	message(STATUS "\nCould not locate CUDA; MCGPU module is disable \n")
endif (${CUDA_FOUND})