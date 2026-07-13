cmake_minimum_required(VERSION 3.14)
project(luo9_sdk_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/bot.cpp
    src/bus.cpp
    src/command.cpp
    src/message.cpp
    src/payload.cpp
    src/pattern.cpp
    src/version.cpp
)

# Header files
set(HEADERS
    include/head.h
    include/luo9_sdk.h
    include/bot.h
    include/bus.h
    include/command.h
    include/message.h
    include/payload.h
    include/pattern.h
    include/version.h
)

# Build static library
add_library(luo9_sdk_cpp STATIC ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(luo9_sdk_cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Platform and compiler specific configuration
if(WIN32)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # MinGW-w64 (GCC on Windows)
        target_compile_options(luo9_sdk_cpp PRIVATE -Wall -Wextra -Wpedantic)
        target_link_libraries(luo9_sdk_cpp PRIVATE pthread)
    elseif(MSVC)
        # MSVC
        target_compile_definitions(luo9_sdk_cpp PRIVATE _CRT_SECURE_NO_WARNINGS)
        target_compile_options(luo9_sdk_cpp PRIVATE /W3 /utf-8)
    endif()
    target_link_directories(luo9_sdk_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows)
    target_link_libraries(luo9_sdk_cpp PUBLIC luo9_core)
else()
    # Linux: GCC/Clang
    target_compile_options(luo9_sdk_cpp PRIVATE -Wall -Wextra -Wpedantic -pthread)
    target_compile_definitions(luo9_sdk_cpp PRIVATE _GNU_SOURCE)
    target_link_directories(luo9_sdk_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux)
    target_link_libraries(luo9_sdk_cpp PUBLIC luo9_core pthread)
    set_target_properties(luo9_sdk_cpp PROPERTIES
        BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux"
        INSTALL_RPATH "\$ORIGIN/../lib"
    )
endif()

# Install rules
install(TARGETS luo9_sdk_cpp
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${HEADERS} DESTINATION include)
