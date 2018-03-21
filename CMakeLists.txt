# Determine the minimum cmake version required for this
cmake_minimum_required(VERSION 2.8)

# Name of the project
project(kdEPD)

# Compiler flags for build
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 ${CMAKE_CXX_FLAGS}")

# Options for enabling / disabling features of the driver
option(COGVERSIONV2 "Set to enabled if COG version driver should be used." ON)
# option(ENABLE_DEBUG_MSG "Set to enable debug prints for low level debugging." OFF)

# External libraries 
set(LIBS4COMPILE 
	pthread
	rt
)

# Include directories
include_directories(
	inc
	images
)

# Linker dependent directories
link_directories(
	${LINK_DIRECTORIES}
)

# Files for compilation
file(GLOB SOURCES
	"src/*.c"
	"src/*.cpp"
)

# Additional defines to be introduced
if (COGVERSIONV2)
    add_definitions ( -DEPD_COG_VERSION=2 )
endif(COGVERSIONV2)

# Executables generated here
add_executable(
	${PROJECT_NAME} 

	${SOURCES}
)

# Linking libraries
target_link_libraries(
	${PROJECT_NAME} 

	${LIBS4COMPILE}
)
