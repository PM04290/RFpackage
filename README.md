# RFpackage
Arduino library, high level class for RF 433MHz TX/RX
Each end point have an address from 1 to 99 and child node from 1 to 99 (input or output)
A gateway have address 0

There is 4 methods to publish/get data :
- PublishNum
- PublishFloat
- PublishText
- PublishTag (for RFID sensor)

Default speed to 2000 is the best compromise between speed and distance; with quality senders/receivers you can try 4000

Because of VirtualWire using, in TX mode there is 2 pin configured : TX and PTT (Push To Talk), so if you don't use PTT, config an unused pin for it.

with examples :
- endpoint Analog sensor (TX) and switch (RX)
- endpoint Switch only (RX)
- Gateway from RFpackage to serial2mqtt (Home Assistant Add-on)
