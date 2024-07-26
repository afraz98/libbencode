set(LIB_NAME "bencode")

set(LIB_VERSION_MAJOR 0)
set(LIB_VERSION_STRING 0.1)

INCLUDE(GNUInstallDirs)
include_directories (include)

set(SRC
    src/bencode.c)

set(HDR
    include/bencode.h)

add_library(${LIB_NAME} SHARED ${SRC})

# Add compiler definitions
add_definitions("-g")

# Install library
install (TARGETS ${LIB_NAME} DESTINATION lib)
