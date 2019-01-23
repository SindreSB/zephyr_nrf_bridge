cmake_minimum_required(VERSION 3.12.1)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(zepthyr_nrf_bridge)

set(ZEPHYR_TOOLCHAIN_VARIANT gnuarmemb)
Set(GNUARMEMB_TOOLCHAIN_PATH C:/gnu_arm_embedded)


FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})