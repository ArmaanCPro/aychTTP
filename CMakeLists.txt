cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES 1)


# ---- Project

project (aychTTP CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---- Executable Definition
add_executable(aychTTP)

target_sources(aychTTP PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tcp_server.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/response_handler.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_helper.cpp"

    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/asio_helper.h"
    #"${CMAKE_CURRENT_SOURCE_DIR}/src/tcp_server.h"
    #"${CMAKE_CURRENT_SOURCE_DIR}/src/response_handler.h"
)
target_sources(aychTTP PUBLIC FILE_SET CXX_MODULES FILES
        src/aych.cppm
        src/http_request.cppm
        src/tcp_server.cppm
        src/response_handler.cppm
        #src/tcp_server.cpp
        #src/response_handler.cpp
)

target_compile_features(aychTTP PUBLIC cxx_std_23)

target_include_directories(aychTTP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# ---- Dependencies

find_package(Boost REQUIRED COMPONENTS asio)
target_link_libraries(aychTTP PRIVATE Boost::asio)

# link liburing on linux for Boost.Asio stream_file
if (UNIX AND NOT APPLE)
    include(FindPkgConfig)
    pkg_check_modules(liburing REQUIRED IMPORTED_TARGET GLOBAL liburing>=2.0)

    target_link_libraries(aychTTP PRIVATE PkgConfig::liburing)
    target_compile_definitions(aychTTP PRIVATE BOOST_ASIO_HAS_IO_URING)
endif ()


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
        BOOST_ASIO_NO_DEPRECATED
        BOOST_ASIO_DISABLE_CONCEPTS
        BOOST_ASIO_HAS_THREADS
        $<$<BOOL:${WIN32}>:_WIN32_WINNT=0x0A00>
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
