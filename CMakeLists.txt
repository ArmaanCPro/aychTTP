cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_STD ON)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD 1)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)


# ---- Project

project (aychTTP CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---- Executable Definition
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/example_module.cpp
)
target_sources(${PROJECT_NAME} PUBLIC FILE_SET CXX_MODULES FILES
    src/example_module.cppm
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# ---- Dependencies

find_package(Boost REQUIRED COMPONENTS asio)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::asio)


# ---- Compiler Flags

set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION ON
)

if (ENABLE_ASAN STREQUAL "ON")
    target_compile_options(${PROJECT_NAME} PRIVATE
        # GCC, Clang
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fsanitize=address,undefined>

        # MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/fsanitize=address>
    )

    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fsanitize=address,undefined>
    )
endif ()

target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /wd4201 /wd4100 /WX /EHsc)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -W -Wall -Werror -Wextra -Wconversion -Wshadow -pedantic -Wno-unused-parameter)
endif ()

