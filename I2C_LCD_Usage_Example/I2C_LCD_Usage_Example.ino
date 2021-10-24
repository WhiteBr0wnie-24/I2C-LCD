#include <Wire.h>

#define LCD_ADDRESS 0x1F

void setup()
{
  Wire.begin(); 

  // You can group command-strings like this
  Wire.beginTransmission(LCD_ADDRESS);
  Wire.write("INIT_16_2");
  Wire.write("CURSOR_0_0");
  Wire.endTransmission();
  delay(100);
  // But you have to send text-strings
  // separately
  Wire.beginTransmission(LCD_ADDRESS);
  Wire.write("Arduino I2C-LCD");
  Wire.endTransmission();
  delay(100);
  // Set the cursor to the next line
  Wire.beginTransmission(LCD_ADDRESS);
  Wire.write("CURSOR_0_1");
  Wire.endTransmission();
  delay(100);
  // Write text to the second line
  Wire.beginTransmission(LCD_ADDRESS);
  Wire.write("It works! :)");
  Wire.endTransmission();
  
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  // Basic heartbeat to show that the
  // Arduino didn't lock up
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500);
}
