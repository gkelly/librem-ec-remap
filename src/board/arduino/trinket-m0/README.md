This is the Arduino code for an I2C debugging adapter based on the Adafruit
Trinket M0. The Trinket will act as I2C client device on address 0x69 and
output received data over USB UART.

To compile under Arduino IDE you need to include the Adafruit board
definition source, select the Trinket M0 board and also add the Adafruit
Dotsar LED library through the library manager.

Wiring is pretty simple, SCL and SDA are marked on the Trinket's silkprint.
Make very sure to have a proper GND connection between Trinket and the
debuggin target. It is also advisable to have all devices (debug target and
debugging host) properlz grounded and earthed to have all devices on the
same level.
