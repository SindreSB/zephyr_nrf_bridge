# nBridge PCB

The nBridge consists of a LiPo charger, a 3.3 regulator, a BL652 BLE module and a CC2500 module

## Parts list
| Designator     | Description                     | Manufacturer number              |   |   |
|----------------|---------------------------------|----------------------------------|---|---|
| R2             | RES SMD 560 OHM 5% 1/16W 0402   | RC0402JR-07560RL                 |   |   |
| C1, C2, C3, C4 | CAP CER 1UF 16V X5R 0402        | CL05A105KO5NNNC                  |   |   |
| J1             | USB TYPE-C, RA RCPT, PCB MOUNT, | 12401610E4#2A                    |   |   |
| R3             | RES SMD 5.6K OHM 5% 1/32W 01005 | ERJ-XGNJ562Y                     |   |   |
| R4, R5         | RES SMD 220 OHM 1% 1/32W 01005  | CRCW0100220RFREL                 |   |   |
| SW1            | SWITCH SLIDE SPDT 100MA 12V     | CSS-1210TB                       |   |   |
| U1             | ULTRA LOW IQ LDO                | TPS7A0533PYKAR                   |   |   |
| U3             | RF TXRX MOD BLUETOOTH CHIP ANT  | BL652-SA-01-T/R                  |   |   |
| R6             | ERJ-XGNJ273Y                    | RES SMD 27K OHM 5% 1/32W 01005   |   |   |
| R1, R7         | RC0402F103CS                    | RES SMD 10K OHM 1% 1/32W 01005   |   |   |
| U2             | BQ25100YFPR                     | BQ25100 SINGLE INPUT LINEAR CHAR |   |   |
| D1             |                                 | RED 0603 LED                     |   |   |
| D2             |                                 | GREEN 0603 LED                   |   |   |




## PCB 
The PCB can be found on https://aisler.net/p/FUKRPVUI. Newer, and maybe untested, revisions might be available there as well. The full source of the PCB is in the folder nBridge, though some symbols might be in different libraries not included. Currently, only revision 1 has been ordered.

## Pinout 
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


