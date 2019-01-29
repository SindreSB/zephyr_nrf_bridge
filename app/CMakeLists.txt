cmake_minimum_required(VERSION 3.12.1)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(zepthyr_nrf_bridge)

set(CMAKE_EXPORT_COMPILE_COMMANDS y)

FILE(GLOB app_sources src/*.c )
target_sources(app PRIVATE ${app_sources})

FILE(GLOB cc_sources src/cc2500/*.c )
target_sources(app PRIVATE ${cc_sources})