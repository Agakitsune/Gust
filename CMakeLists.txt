cmake_minimum_required(VERSION 3.22)
project(gust)

enable_language(C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Make a flag to build with sanitize
set(SANITIZE ON CACHE BOOL "Enable sanitize")

file(GLOB_RECURSE SOURCE
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.s
)

file(GLOB_RECURSE SOURCECPP
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCE})
add_library(${PROJECT_NAME}++ SHARED ${SOURCECPP})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

if (SANITIZE)
    set(SANITIZE_LIBS
        asan
        ubsan
    )
    set(SANITIZE_FLAGS
        -fsanitize=address
        -fsanitize=undefined
        -fsanitize=leak
        -fno-omit-frame-pointer
    )
endif()

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ${SANITIZE_LIBS}
)

target_link_libraries(${PROJECT_NAME}++
    PUBLIC
        ${PROJECT_NAME}
        ${SANITIZE_LIBS}
)

message(STATUS "Building in ${CMAKE_BUILD_TYPE}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    if (LINUX AND NOT APPLE)
    target_compile_options(${PROJECT_NAME}
        PUBLIC
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -Wno-unused-parameter
            -Wno-unused-variable
            -Wno-unused-but-set-variable
            -Wno-unused-function
            -Wno-unused-result
            -Wno-unused-value
            -Wno-unused-label
            -Wno-unused-local-typedefs
            -Wno-unused-macros
            -Wno-unused-const-variable
    )

    target_compile_options(${PROJECT_NAME}++
        PUBLIC
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -Wno-unused-parameter
            -Wno-unused-variable
            -Wno-unused-but-set-variable
            -Wno-unused-function
            -Wno-unused-result
            -Wno-unused-value
            -Wno-unused-label
            -Wno-unused-local-typedefs
            -Wno-unused-macros
            -Wno-unused-const-variable
    )
    endif()
else()
    message(STATUS "Sanitize: ${SANITIZE}")
    message(STATUS "Sanitize flags: ${SANITIZE_FLAGS}")
    message(STATUS "Sanitize libs: ${SANITIZE_LIBS}")

    if (LINUX AND NOT APPLE)
        target_compile_options(${PROJECT_NAME}
            PUBLIC
                -ggdb
                -g3
                -Wall
                -Wextra
                -Wpedantic
                ${SANITIZE_FLAGS}
        )
    endif()

    if (EXISTS ${CMAKE_SOURCE_DIR}/main.c)
        add_executable(${PROJECT_NAME}_demo
            main.c
        )

        target_link_libraries(${PROJECT_NAME}_demo
            PUBLIC
                ${PROJECT_NAME}
                ${SANITIZE_LIBS}
        )

        if (LINUX AND NOT APPLE)
            target_compile_options(${PROJECT_NAME}_demo
                PUBLIC
                    -ggdb
                    -g3
                    -Wall
                    -Wextra
                    -Wpedantic
                    ${SANITIZE_FLAGS}
            )
        endif()
    endif()

    if (EXISTS ${CMAKE_SOURCE_DIR}/main.cpp)
        add_executable(${PROJECT_NAME}_demo++
            main.cpp
        )

        target_link_libraries(${PROJECT_NAME}_demo++
            PUBLIC
                ${PROJECT_NAME}++
                ${SANITIZE_LIBS}
        )

        if (LINUX AND NOT APPLE)
            target_compile_options(${PROJECT_NAME}_demo++
                PUBLIC
                    -ggdb
                    -g3
                    -Wall
                    -Wextra
                    -Wpedantic
                    ${SANITIZE_FLAGS}
            )
        endif()
    endif()
endif()
