cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ---- Project

project (aychTTP CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---- Executable Definition
add_executable(aychTTP)

target_sources(aychTTP PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tcp_server.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/response_handler.cpp"

    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tcp_server.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/response_handler.h"
)

target_compile_features(aychTTP PUBLIC cxx_std_23)

target_include_directories(aychTTP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# ---- Dependencies

find_package(Boost REQUIRED COMPONENTS asio)
target_link_libraries(aychTTP PRIVATE Boost::asio)


# ---- Compiler Flags

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(aychTTP PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION ON
    )
else()
    set_target_properties(aychTTP PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION OFF
    )
endif()

if (ENABLE_ASAN STREQUAL "ON")
    target_compile_options(aychTTP PRIVATE
        # GCC, Clang
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fsanitize=address,undefined>

        # MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/fsanitize=address>
    )

    target_link_options(aychTTP PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fsanitize=address,undefined>
    )
endif ()

target_compile_definitions(aychTTP PRIVATE
        $<$<BOOL:${WIN32}>:NOMINMAX;_CRT_SECURE_NO_WARNINGS;WIN32_LEAN_AND_MEAN;_WIN32_WINDOWS>
)

target_compile_options(aychTTP PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4;/wd4201;/wd4100;/WX;/EHsc>

        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
            -W;-Wall;-Werror;-Wextra;-Wconversion;-Wshadow;-pedantic;-Wno-unused-parameter
        >
)

add_custom_command(TARGET aychTTP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/test_client"
        "$<TARGET_FILE_DIR:aychTTP>/test_client"
        COMMENT "Copying test_client to binary directory."
    VERBATIM
)
