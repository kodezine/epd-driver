# Determine the minimum cmake version required for this
cmake_minimum_required(VERSION 3.9)

# Name of the project
project(epd_test C CXX)

# Compiler flags for build
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -D_FILE_OFFSET_BITS=64 ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-Wall -D_FILE_OFFSET_BITS=64 ${CMAKE_C_FLAGS}")
# Options for enabling / disabling features of the driver
option(COGVERSIONV2 "Set to enabled if COG version driver should be used." ON)
option(NETID_TEST "Set to build for testing network identificatin program." OFF)

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
set(SOURCES
	"${CMAKE_SOURCE_DIR}/src/epd_v2.c"
	"${CMAKE_SOURCE_DIR}/src/gpio.c"
	"${CMAKE_SOURCE_DIR}/src/spi.c"
)


# Additional defines to be introduced
if (COGVERSIONV2)
    add_definitions ( -DEPD_COG_VERSION=2 )
endif(COGVERSIONV2)

# Executables generated here
add_executable(${PROJECT_NAME} 
	${SOURCES}
	${CMAKE_SOURCE_DIR}/src/epd_test.c
)

# Linking libraries
target_link_libraries(${PROJECT_NAME} 
	${LIBS4COMPILE}
)
add_executable(epd_fuse 
	${SOURCES}
	${CMAKE_SOURCE_DIR}/src/epd_fuse.c
)
target_link_libraries(epd_fuse 
	${LIBS4COMPILE}
	fuse
)
if (NETID_TEST)

	add_definitions( -DTEST_EPD_NETID )

	add_executable(epd_netID 
		${CMAKE_SOURCE_DIR}/src/epd_netid.c
	)
else()
	add_executable(epd_app 
		${CMAKE_SOURCE_DIR}/src/epd_app.c
		${CMAKE_SOURCE_DIR}/src/epd_fonts.c
		${CMAKE_SOURCE_DIR}/src/epd_netid.c
	)
endif()
