# Zephyr based bridge for Dexcom G4

Highly WIP, and not at all working

## Installation
This repo contains a west manifest file, which enables this to be a manifest repo when using the west tool.

Steps to get up and running:
1. Follow install guide for zephyr/west 
1. Create a folder to hold the project and zephyr etc
1. Insted of running west init, use west init -m \<url to this repo>
1. Use west build with the propper arguments and -s zephyr_nrf_bridge/app