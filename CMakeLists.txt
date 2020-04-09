cmake_minimum_required(VERSION 3.1)

project(cuda-additive-synthesis)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Enable C++11 for host code
set(CMAKE_CXX_STANDARD 11)

list(APPEND CUDA_NVCC_FLAGS_DEBUG -G -g)
list(APPEND CUDA_NVCC_FLAGS_RELWITHDEBUGINFO -lineinfo)

# Crucial magic for CUDA linking
find_package(Threads REQUIRED)
find_package(CUDA 10 REQUIRED)

set(CUDA_ATTACH_VS_BUILD_RULE_TO_CUDA_FILE ON)
set(CUDA_SEPARABLE_COMPILATION ON)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CUDA_PROPAGATE_HOST_FLAGS OFF)
endif()

include_directories(.)
#find_library(RTAUDIO RtAudio REQUIRED)
#find_path(RTAUDIO_DIR RtAudio.h REQUIRED)
#include_directories("C:/Program Files (x86)/RtAudio/include/")
include_directories(karnel)
cuda_add_library(kernel
"karnel/kernel.cu"
	"karnel/kernel.h"
	OPTIONS -arch=sm_53
	)

cuda_add_executable(${CMAKE_PROJECT_NAME}
    	"src/main.cpp"
        "src/testing_helpers.hpp"
        "src/RtAudio.cpp"
      	"src/RtAudio.h"
        "src/ADSR.cpp"
        "src/ADSR.h"
        "src/lfo.cpp"
        "src/lfo.h"
        "src/engine.h"
        "src/engine.cpp"
    )
SET(GCC_COVERAGE_LINK_FLAGS "-lasound")
find_package(ALSA REQUIRED)
include_directories(${ALSA_INCLUDE_DIRS})
if(WIN32)
    # Set up include and lib paths
    set(CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER} CACHE FILEPATH "Host side compiler used by NVCC" FORCE)
endif(WIN32)
link_directories(${CUDA_TOOLKIT_ROOT_DIR}/lib64)
#"C:/Program Files (x86)/RtAudio/lib/rtaudio.lib"
target_link_libraries(${CMAKE_PROJECT_NAME}
    kernel
    ${CORELIBS}
    ${ALSA_LIBRARIES}
    )
