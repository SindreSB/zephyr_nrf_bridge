To run on NRF52-DK
1) set ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
1) set GNUARMEMB_TOOLCHAIN_PATH=C:\gnu_arm_embedded
1) west build -b nrf52_pca10040
1) west flash

On nrf52840-dongle (requires nrfutil in path):
1) set ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
1) set GNUARMEMB_TOOLCHAIN_PATH=C:\gnu_arm_embedded
1) west build -b nrf52840_pca10059
1) nrfutil pkg generate --hw-version 52 --sd-req=0x00 --application build/zephyr/zephyr.hex --application-version 1 pkg.zip
1) nrfutil dfu usb-serial -pkg pkg.zip -p COM6

On custom boards (out-of-tree):
1) set ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
1) set GNUARMEMB_TOOLCHAIN_PATH=C:\gnu_arm_embedded
1) west build -b \<board\> -- -DBOARD_ROOT="."
1) west flash