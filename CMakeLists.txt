cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

# Specify the name of the executable.
add_executable(assign02)

project(assign02 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_APPS_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

include(apps_auto_set_url.cmake)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

# Specify the source files to be compiled.
target_sources(assign02 PRIVATE assign02.c assign02.S )

# Pull in commonly used features.
target_link_libraries(assign02 PRIVATE pico_stdlib pico_multicore hardware_pio)


# Generate the PIO header file from the PIO source file.
pico_generate_pio_header(assign02 ${CMAKE_CURRENT_LIST_DIR}/assign02.pio)

# Create map/bin/hex file etc.
pico_add_extra_outputs(assign02)

# Add the URL via pico_set_program_url.
apps_auto_set_url(assign02)