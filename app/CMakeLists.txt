cmake_minimum_required(VERSION 3.12.1)

# Re-direct the directory where the 'boards' directory is found from
# $ZEPHYR_BASE to this directory.
#set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(zepthyr_nrf_bridge)

set(CMAKE_EXPORT_COMPILE_COMMANDS y)

FILE(GLOB app_sources src/*.c )
FILE(GLOB dex_sources src/dexcom/*.c )
FILE(GLOB cc_sources src/cc2500/*.c )
FILE(GLOB gatt_sources src/gatt/*.c )

target_sources(app PRIVATE 
    ${app_sources} 
    ${dex_sources} 
    ${cc_sources}
    ${gatt_sources}
) 
