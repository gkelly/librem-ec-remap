// SPDX-License-Identifier: GPL-3.0-only

/*
 * based on DotStart Trinket example (c) Adadfruit industries
 * I2C code added by
 * (c) Nicole Faerber <nicole.faerber@puri.sm>
 * License: GPL-v3
 */

#include <Wire.h>
#include <Adafruit_DotStar.h>

#define NUMPIXELS 1 // Number of LEDs in strip
// Here's how to control the LEDs from any two pins:
#define DATAPIN    7
#define CLOCKPIN   8
Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

#define I2CADDR 0x76

#define I2CCLIENT 1

void receiveEvent(int bytes) {
char c;
  c = Wire.read();    // read one character from the I2C
  //Serial.print("i2c recv 0x");
  //Serial.println(c, HEX);
  Serial.write(c);
}

// the setup function runs once when you press reset or power the board
void setup() {
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

  //pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  //pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW

  Serial.begin(115200);

#ifdef I2CCLIENT
  Wire.begin(I2CADDR);
  Wire.onReceive(receiveEvent);
#else
  Wire.begin();
#endif
}

int      head  = 0, tail = -10; // Index of first 'on' and 'off' pixels
uint32_t color = 0x200000;      // 'On' color (starts red)


// the loop function runs over and over again forever
void loop() {
unsigned char e;
#if 0
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
#endif
#ifdef I2CCLIENT
  // Serial.println("alive...");
#else
  Wire.beginTransmission(I2CADDR); // transmit to device #88
  Wire.write(0xaa);              // sends 0xaa
  e=Wire.endTransmission();
  if (e != 0) {
    Serial.print("Wire tramission fail: ");
    switch (e) {
      case 1: Serial.println("data too long to fit in transmit buffer"); break;
      case 2: Serial.println("received NACK on transmit of address"); break;
      case 3: Serial.println("received NACK on transmit of data"); break;
      case 4: Serial.println("other error"); break;
      default: Serial.println("unknown error");
    }
  } else
    Serial.println("I2C tx OK");
#endif

  strip.setPixelColor(0, color); // 'On' pixel at head
  strip.show();
  color > 0 ? color >>= 8 : color = 0x200000;

  delay(100);
}
