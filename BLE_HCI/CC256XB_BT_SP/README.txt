CC256XB Bluetooth Service Pack - (CC256XB-BT-SP)

v1.6 - December 2016
	- Recommended
	- Bug fixes:
1) When there was already one or more BLE connection active for at least 12 hours, connecting another BLE device resulted in a non-optimal BLE 
   connection offset ('d'). This bug caused a lag in BLE data which was sent only after the link supervision timeout. 
2) BLE disconnection might have occurred after several minutes if an already existing BLE connection in parallel with an ACL connection (slave mode)
   was present.

===========================================================================================================================================================

v1.5 - June 2016
	- Bug Fixes:
1) On certain race conditions, the device could lockup while BLE and ACL connections were active, and another BLE connection was being established.
2) For hosts working with SW flow control, there could be a missed tx packet during excessive BLE continuous transmission.

===========================================================================================================================================================

Previous version details : http://processors.wiki.ti.com/index.php/CC256x_Downloads