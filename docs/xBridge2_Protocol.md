# xBridge2 Protocol spesification
The xBridge2 protocol is used by LBridge as well as Others? to transmit BG measurements to the xDrip+ application. Any transmitter using this protocol can use the Bluetooth Wixel collector in xDrip+. 

## Service and characteristics
The xBridge2 protocol consists of a single service, with one characteristic. 

xBridge2 service 
UUID: 0000ffe0-0000-1000-8000-00805f9b34fb

Communications characteristic: 
UUID: 0000ffe1-0000-1000-8000-00805f9b34fb
Permissions: Read, Write, Notify

## Packets
The following packet types are part of the xBridge2 protocol:

### Measurement record
The measurement record is sent for every new measurement received, as well as when sendingn historic measurements.

Name|Type|Size [bytes]|Value|Description
--- | --- | --- | --- | --- 
Size|Uint8|1 [0]|0x15|Size of the packet - should be 21
Packet type|Uint8|1 [1]|0x00|Code identifying data packet. Always 00 for a Dexcom data packet
Raw|Uint32|4 [2:5]||Raw ISIG value
Filtered|Uint32|4 [6:9]||Filtered ISIG value
Transmitter battery|Uint8|1 [10]||Transmitter battery value, as received from transmitter
Bridge battery|Uint8|1 [11]||Battery percentage 0-100 as an integer
Transmitter id|Uint32|4 [12:15]||Transmitter id of the Dexcom transmitter
Delay|Uint32|4 [16:19]||Offset since measurement in milliseconds?, only positive values makes sense here.
Protocol level|Uint8|1 [20]|0x01|Byte representing xBridge functionality, xBridge2 = 0x01

NOTE: Protocol level is ignored by xDrip and since a standard BLE package is 20bytes, we simply do not send it. 

See lines 1690 in DexCollection service, but nowhere is function used. Size however must be 0x15 to be recognized as a Dexbridge/xBridge packet, se TransmissionData#create. A size 0x11 represents a xBridge1 packet(?), without support for historic data.

Due to C alignment, a C struct with the above types will have the wrong size and byte positions. Such a struct must be "packed" for the decoding in xDrip+ to work.

This packet should be used for both for current and historic measurements. For historic measurements, the raw, filtered and delay are the only fields that change, the Transmitter battery, Bridge battery and Transmitter id should always contain up to date information. 

### Measurement acknowledgement
The acknowledgement package is sent after a measurement is received. 

Name|Type|Size|Value|Description
--- | --- | --- | --- | --- 
Size|Uint8|1 [0]|0x02|Number of bytes in the packet - will always be 2
Packet type|Uint8|1 [1]|0xF0|Packet type- should be 0xF0

Historically the Wixel went to sleep upon reception, or after a 3 sec timeout, but the xDrip app can support receiving another measurement after an acknowledgement has been sent. 

## TxId packet
Name|Type|Size [bytes]|Value|Description
--- | --- | --- | --- | --- 
Size|Uint8|1 [0]|0x06|Number of bytes in package
Packet type|Uint8|1 [1]|0x01|Packet type- txid is 0x01
Transmitter Id|Uint32|4 [2:5]||Dexcom transmitter id, int encoded


##  Beacon package
The beacon package is sent from the app to device

Name|Type|Size [bytes]|Value|Description
--- | --- | --- | --- | --- 
Size|Uint8|1 [0]|0x07|Number of bytes in packet 
Packet type|Uint8|1 [1] |0xF1|Packet type - beacon type is 0xF1
TxId|Uint32|4 [2:5]||Dexcom transmitter id, int encoded
Protocol level|Uint8|1 [6]|0x01|xBridge protocol level

NOTE: In the original xBridge2 documentation, there is a typo which states the size of the beacon package is 0x06, though xBrip correctly uses the size 0x07 to identify a beacon package.

## Conversion from transmitter id to int representation:
Define the table [0-9A-Y], which contains 32 characters. To index into this table, 5 bits are needed. To convert to, from string, each letter is replaced by its index in the table. The resulting ID is 25 bits long, and is stored in a Uint32, all other bits being 0. 

## Sending historic data to xDrip - special note
The protocol itself does not impose any restirctions on when a value is sent, but the xDrip+ app discards values older than the last value received. This means that if you are using the historic measurements functionality, you MUST send the oldest value first. The sender thus have to buffer measurements, and upon connection, transmit the oldest value first. xDrip also ignore measurements within two minutes of another measurement, as well as measurements from before the last calibration. No error will be indicated to the sender in any of these events.
