To run on NRF52-DK
1) Run the zephyr-env.cmd cmdlet to set env variables
2) set ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
3) set GNUARMEMB_TOOLCHAIN_PATH=C:\gnu_arm_embedded
4) cmake -GNinja -DCONF_FILE=prj.conf -DBOARD=nrf52_pca10040 ..
5) ninja
6) ninja flash