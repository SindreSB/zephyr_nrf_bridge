# nBridge PCB

The nBridge consists of a LiPo charger, a 3.3 regulator, a BL652 BLE module and a CC2500 module

## Versions
V1: Base design. Used very small parts which are too difficult to reliably solder
V2: Lower part count with larger components, as well as micro-USB instead of USB-C type connector.

## Parts list
The partslist is still changing, and it is easier to export it from the most recent desgin files than to have possibly outdatet information here. All passives are 0804 size componetns, while hte regulator and LDO are SOT23. The LEDs are 0603. 


## PCB 
The PCB can be found on https://aisler.net/p/FUKRPVUI. Newer, and maybe untested, revisions might be available there as well. The full source of the PCB is in the folder nBridge, though some symbols might be in different libraries not included. Currently, only revision 1 has been ordered.

## Pinout (V1)
### NRF52 - CC2500
| BL652 pin(V1) | nRF52832 pin | Function | 
|----|-|-----|
| 31 | P0.19 | CC2500 - MOSI | 
| 32 | P0.31 | CC2500 - CLK |
| 33 | P0.30 | CC2500 - MISO |
| 35 | P0.28 | CC2500 - GDO0 |
| 36 | P0.27 | CC2500 - GDO2 |
| 37 | P0.26 | CC2500 - CSn |
| 34 | P0.29/AIN5 | Analog input of battery voltage |
| 38 | P0.25 | Enable battery voltage measurement |
| 20 | P0.05 | LED0 |
| 19 | P0.06 | LED1 |
| 5  | SWDIO | SWDIO |
| 6  | SWDCLK | SWDCLK |
| 7  | P0.21 | nRST |
| 9  | P0.18 | SWO |


