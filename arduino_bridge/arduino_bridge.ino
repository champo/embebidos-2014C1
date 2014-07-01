

// Wire Slave Sender
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>


void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Wire.begin(0xA);                // join i2c bus with address 0x

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{

  if (Serial.available() >= 3) {

        digitalWrite(LED_BUILTIN, HIGH);

    int address = Serial.read();
    int cmd = Serial.read();
    int type = Serial.read();

    Serial.write('a');
    return;

    Wire.beginTransmission(address);
    Wire.write(cmd);
    Wire.write(type);

    if (cmd == 0x0) {

      while (Serial.available() == 0);
      Wire.write(Serial.read());

      Wire.endTransmission();

    } else {

      Wire.endTransmission(false);
      Wire.requestFrom(address, 1, true);

      while (Wire.available() == 0);
      Serial.write(Wire.read());
    }

  }
}
