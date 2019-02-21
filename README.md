# Zephyr based bridge for Dexcom G4

Highly WIP, and not at all working

## Installation
This repo contains a west manifest file, which enables this to be a manifest repo when using the west tool.

Steps to get up and running:
1. Follow install guide for zephyr/west
1. Clone this repo 
1. Set the variables:
    * ZEPHYR_BASE=\<path to zephyr>
    * ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
    * GNUARMEMB_TOOLCHAIN_PATH=\<path to gnu>
1. cd into the app directory
1. Use west build with the propper arguments and -s .
    * west build -b nrf52_pca10040 -s .