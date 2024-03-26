# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/Users/luis/Code/personal/pico/pico-sdk")

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(devdeck-monitor C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(devdeck-monitor)

pico_generate_pio_header(devdeck-monitor
        ${CMAKE_CURRENT_LIST_DIR}/smd.pio
)

target_sources(devdeck-monitor PRIVATE
        devdeck-monitor.cpp
        life.h
        life.cpp
        smd.h
        smd.cpp
)

pico_set_program_name(devdeck-monitor "devdeck-monitor")
pico_set_program_version(devdeck-monitor "0.1")

pico_enable_stdio_uart(devdeck-monitor 1)
pico_enable_stdio_usb(devdeck-monitor 0)

# Add any user requested libraries
target_link_libraries(devdeck-monitor PRIVATE
        pico_stdlib
        hardware_pio
        hardware_adc
)

pico_add_extra_outputs(devdeck-monitor)

