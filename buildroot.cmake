# Important to specify system name
SET(CMAKE_SYSTEM_NAME Linux)

# Not so important to specify version number
SET(CMAKE_SYSTEM_VERSION 1)

# Specify location of toolchain root folder
SET(TC_ROOT_FOLDER "/home/p/work/buildroot.git/output/host")
# Specify toolchain prefix
SET(TC_PREFIX "armeb-buildroot-linux-gnueabihf")
# Specify cross compiler 
SET(CMAKE_C_COMPILER    ${TC_ROOT_FOLDER}/bin/${TC_PREFIX}-gcc)
SET(CMAKE_CXX_COMPILER  ${TC_ROOT_FOLDER}/bin/${TC_PREFIX}-g++)

# Specify sysroot for toolchain
SET(CMAKE_SYSROOT ${TC_ROOT_FOLDER}/${TC_PREFIX}/sysroot)

# Specify the root path
SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_ROOT_FOLDER})

# Search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

