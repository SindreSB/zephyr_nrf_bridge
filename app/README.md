To run on NRF52-DK
1) Run the zephyr-env.cmd cmdlet to set env variables
2) set ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
3) set GNUARMEMB_TOOLCHAIN_PATH=C:\gnu_arm_embedded
4) cmake -GNinja -DCONF_FILE=prj.conf -DBOARD=nrf52_pca10040 ..
5) ninja
6) ninja flash

On nrf52840-dongle (requires nrfutil in path):
1) set ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
2) set GNUARMEMB_TOOLCHAIN_PATH=C:\gnu_arm_embedded
3) west build -b nrf52840_pca10059
4) nrfutil pkg generate --hw-version 52 --sd-req=0x00 --application build/zephyr/zephyr.hex --application-version 1 pkg.zip
5) nrfutil dfu usb-serial -pkg pkg.zip -p COM6