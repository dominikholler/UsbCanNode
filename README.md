# UsbCanNode for infineon XMC4700 
This software provides some of the XMC4700's MultiCAN features to a spartan command line terminal over USB. The software is ready-to-use on a XMC4700 Relax Kit (KIT_XMC47_RELAX_V1).

## Getting Started
1. Import this projet in the free-of-charge infineon DAVE™ (Version 4)  IDE.
2. Connect USB cable from PC to debugging Debugger Micro USB plug, the one close to the XMC 4200 debugger IC, of the board.
3. Flash the software by starting debugging this project in DAVE.
4. Unplug the USB cable from the Debugger Micro USB and plug in the other Micro USB plug, the one close to the XMC4700.
5. The XMC tries to boot and the LED2 will light up. Windows will detect new hardware, provide usbCanNode/Dave/Generated/USBD_VCOM as location for the driver. If installing the driver was successful, the LED2 will be deactivated.
6. Use any serial terminal application, e.g. putty, to connect to the appropriat serial COM port, e.g. COM3, with a rate of 115200.
7. Send a Return to get the promt `>` in the terminal.

## Commands
### Examples
```
> Echo on
Result: Success
Received 0x400 3 0x1 0x2 0x3
Received 0x400 3 0x1 0x2 0x3
> Send active 2 0x400 0x1 02 3
Result: Success
Activation by button
Deactivation by button
> Send 0:56 surge
```
### Reference
#### Analyzer
`Analyzer [on|off]`
Enables or disables read-only passive mode.
### Button
Pressing Button1 first time `<condition>` for sending is set to active. This may trigger the last given send command. Pressing Button1 during sending, the sending is deactivated by setting `<condition>` for sending to passive.
#### Echo
`Echo [on|off]`
Enables or disables printing the last recvied frame. Since printing consumes a lot of CPU time, frames recived with a high freqency may be not shown and sending of frames is slowed down. For this reasons Echo if off initally after power on.
### Send
`Send <condition> [<count> <CAN-ID> <data> | surge]`
Send CAN frames. Sending may be triggered by a `<condition>`. `<condition>` may be 
- `active`, which triggers instantly,
- `passive`, which could by triggerd by Button1
- `<CAN-ID:BitPostion>`, which activates if bit on position `BitPostion` is set in recived CAN frame with `CAN-ID`, and deactivates if this bit is not set. Bits in CAN frame are counted in LSB 0 like 
```
	7  6  5  4  3  2  1  0
	15 14 13 12 11 10  9  8
	 23 22 21 20 19 18 17 16
	 31 30 29 28 27 26 25 24
	 39 38 37 36 35 34 33 32
	 47 46 45 44 43 42 41 40
	 55 54 53 52 51 50 49 48
	 63 62 61 60 59 58 57 56
	```
### Surge
`Surge`
Sends a surge of CAN frames from high to low priority.
