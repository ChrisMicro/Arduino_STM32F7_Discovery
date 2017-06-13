#include "Wire.h"
#include "Adafruit_FT6206.h" // library manager, search for FT6206

Adafruit_FT6206 touch = Adafruit_FT6206();

void setup() {
  Serial.begin(115200);

  for (int n = 0; n < 5; n++)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("start touch");

  Wire.stm32SetInstance(I2C3);
  Wire.stm32SetSDA(PH8);
  Wire.stm32SetSCL(PH7);

  touch.begin();
}

int Counter = 0;

void loop() {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    Serial.print(Counter++);
    Serial.print(": ");
    Serial.print(p.x);
    Serial.print(" ");
    Serial.println(p.y);
  }
}

