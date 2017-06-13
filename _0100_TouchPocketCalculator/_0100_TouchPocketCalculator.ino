/*
   Touch Screen Pocket Calculator Example
   for STM32F476 Discovery

   This example uses the reverse polish notaion like the
   old HP-Calculators

   https://en.wikipedia.org/wiki/Reverse_Polish_notation

   Library Dependencies:
   * Import the Adafruit GFX library
   * Sketch=>Include Libraries=>Manage Libraries=>Adafruit GFX

   June 2017, ChrisMicro

*/

#include <Wire.h>
#include "stm32_ub_touch_480x272.h"
#include "colors.h"
#include "LTDC_F746_Discovery.h"

LTDC_F746_Discovery tft;

uint8_t UB_I2C3_ReadByte(uint8_t addressI2cDevice, uint8_t registerId)
{
  uint8_t result;
  addressI2cDevice = addressI2cDevice >> 1;

  Wire.beginTransmission( addressI2cDevice  );
  Wire.write( registerId );
  uint8_t error;
  error = Wire.endTransmission();

  Wire.requestFrom( addressI2cDevice, (uint8_t) 1 , (uint8_t) true );

  while ( Wire.available() < 1 );

  result = Wire.read() ;

  if (error)Serial.println("I2C error");

  return result;
}

#define BUTTONNAME_MAXLENGTH 8

class Button
{
    uint16_t posX, posY;

    char buttonName[BUTTONNAME_MAXLENGTH];
    uint16_t colorDefault;
    uint16_t color;
    uint8_t state;

  public:
    uint16_t width, height;
    Button(uint16_t x, uint16_t y, char * btnName)
    {
      width = 50;
      height = 40;
      posX = x;
      posY = y;
      strncpy(buttonName, btnName, BUTTONNAME_MAXLENGTH);
      colorDefault = COLOR_GREY;
      color = colorDefault;
      state = 0;
      uint16_t w=strlen(btnName)*20+10;
      if(w>width) width=w;
    }

    void show()
    {
      tft.fillRect(posX, posY, width, height, color);
      tft.setCursor(posX + 10, posY + 10);
      tft.setTextColor(COLOR_BLUE);
      tft.print(buttonName);
    }

    boolean isHit()
    {
      boolean flag = false;
      int x = Touch_Data.xp;
      int y = Touch_Data.yp;
      if ( x > posX && x < posX + width && y > posY && y < posY + height ) flag = true;
      if (flag) color = COLOR_ORANGE;
      else color = colorDefault;
      show();
      return flag;
    }

    boolean wasPressed()
    {
      boolean wasPressed_flag = false;
      switch (state)
      {
        case 0:
          {
            if (isHit()) state = 1;
          } break;
        case 1:
          {
            if (!isHit())
            {
              state = 0;
              wasPressed_flag = true;
            }
          } break;

      }
      return wasPressed_flag;
    }

};

#define KEYPADX 10
#define KEYPADY 100
#define SPACEY 60
#define SPACEX 60

Button button7      (KEYPADX + SPACEX * 0, KEYPADY + SPACEY * 0, "7");
Button button8      (KEYPADX + SPACEX * 1, KEYPADY + SPACEY * 0, "8");
Button button9      (KEYPADX + SPACEX * 2, KEYPADY + SPACEY * 0, "9");
Button button4      (KEYPADX + SPACEX * 0, KEYPADY + SPACEY * 1, "4");
Button button5      (KEYPADX + SPACEX * 1, KEYPADY + SPACEY * 1, "5");
Button button6      (KEYPADX + SPACEX * 2, KEYPADY + SPACEY * 1, "6");
Button button1      (KEYPADX + SPACEX * 0, KEYPADY + SPACEY * 2, "1");
Button button2      (KEYPADX + SPACEX * 1, KEYPADY + SPACEY * 2, "2");
Button button3      (KEYPADX + SPACEX * 2, KEYPADY + SPACEY * 2, "3");
Button button0      (KEYPADX + SPACEX * 3, KEYPADY + SPACEY * 2, "0");

Button buttonEnter  (KEYPADX * 2 + SPACEX * 4, KEYPADY + SPACEY * 0, "Enter");
Button buttonClear  (KEYPADX * 2 + SPACEX * 6, KEYPADY + SPACEY * 0, "CLR");

Button buttonPlus   (KEYPADX * 2 + SPACEX * 4, KEYPADY + SPACEY * 1, "+");
Button buttonMinus  (KEYPADX * 2 + SPACEX * 5, KEYPADY + SPACEY * 1, "-");
Button buttonMul    (KEYPADX * 2 + SPACEX * 4, KEYPADY + SPACEY * 2, "*");
Button buttonDiv    (KEYPADX * 2 + SPACEX * 5, KEYPADY + SPACEY * 2, "/");

int Old_x = 0, Old_y = 1;

int getButton()
{
  int returnValue = -1;

  do
  {
    UB_Touch_Read();

    int x = Touch_Data.xp;
    int y = Touch_Data.yp;

    if (Old_x != x || Old_y != y)
    {
      if (button1.wasPressed()) returnValue = '1';
      if (button2.wasPressed()) returnValue = '2';
      if (button3.wasPressed()) returnValue = '3';
      if (button4.wasPressed()) returnValue = '4';
      if (button5.wasPressed()) returnValue = '5';
      if (button6.wasPressed()) returnValue = '6';
      if (button7.wasPressed()) returnValue = '7';
      if (button8.wasPressed()) returnValue = '8';
      if (button9.wasPressed()) returnValue = '9';
      if (button0.wasPressed()) returnValue = '0';
      if (buttonEnter.wasPressed()) returnValue = 'e';
      if (buttonClear.wasPressed()) returnValue = 'c';
      if (buttonPlus.wasPressed()) returnValue = '+';
      if (buttonMinus.wasPressed()) returnValue = '-';
      if (buttonMul.wasPressed()) returnValue = '*';
      if (buttonDiv.wasPressed()) returnValue = '/';
     // Old_x = x;
     // Old_y = y;
    }
  } while (returnValue == -1);

  return returnValue;

}


void setup()
{
  Wire.stm32SetInstance(I2C3);
  Wire.stm32SetSDA(PH8);
  Wire.stm32SetSCL(PH7);

  Wire.begin();

  UB_Touch_Init();

  // The buffer is memory mapped
  // You can directly draw on the display by writing to the buffer
  uint16_t *buffer = (uint16_t *)malloc(LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height);

  tft.begin((uint16_t *)buffer);

  tft.fillScreen(LTDC_BLACK);
  //tft.setRotation(0);
  tft.setCursor(0, 0);
  tft.setTextColor(LTDC_BLUE);  tft.setTextSize(3);
  tft.println("STM32F746 Calculator");

}

int Counter = 0;
int KeyCounter = 0;
#define KEYBUFFERLENGTH 8
char KeyBuffer[KEYBUFFERLENGTH];

void showKeyBuffer()
{
  tft.fillScreen( LTDC_BLACK );
  tft.setCursor(0, 0);
  for (int n = 0; n < KeyCounter; n++)
  {
    tft.print(KeyBuffer[n]);
  }
}

double getNumberFromKeyBuffer()
{
  double value;
  if (KeyCounter == 0)
  {
    return 0;
  } else
  {
    value = KeyBuffer[0] - '0';
    for (int n = 1; n < KeyCounter; n++)
    {
      value = value * 10 + KeyBuffer[n] - '0';
    }
  }
  return value;
}

#define NUMBERSTACKLENGTH 3
int NumberStackCounter = 0;
double NumberStack[NUMBERSTACKLENGTH];

void printStack()
{
  tft.fillScreen( LTDC_BLACK );

  tft.setCursor(0, 10);
  tft.println("stack:");

  for (int n = 0; n < NumberStackCounter; n++)
  {
    tft.println(NumberStack[n]);
  }
  delay(1000);
}

void push(double value)
{
  if (NumberStackCounter == NUMBERSTACKLENGTH)
  {
    // to much stack entries, discard the lowest one:
    for(int n=2;n<NUMBERSTACKLENGTH;n++) NumberStack[n-1]=NumberStack[n];
    NumberStackCounter--;
  }

   NumberStack[NumberStackCounter++] = value;
   
}

double pushKeyBuffer()
{
  double value=0;
  
  value=getNumberFromKeyBuffer();
  push(value);

  return value;
}

double pop()
{
  double value=0;
  if(NumberStackCounter>-1)
  {
    value = NumberStack[--NumberStackCounter];
  }
  return value;

}

void loop()
{
  static double result=0;
  
  int button = getButton();

  if (button >= '0' && button <= '9')
  {
    if (KeyCounter < KEYBUFFERLENGTH)
    {
      KeyBuffer[KeyCounter++] = button;
    }
  }

  // enter number, push it to stack
  if (button == 'e')
  {
    result=pushKeyBuffer();
    KeyCounter = 0; // clear buffer
  }

  if (button == '+')
  {
    double a,b;
    b=pop();
    if( KeyCounter > 0 ) a = pushKeyBuffer();
    else a=pop();
    
    KeyCounter = 0; // clear buffer

    result=a+b;
    push(result);
  }

  if (button == '-')
  {
    double a,b;
    b=pop();
    if( KeyCounter > 0 ) a = pushKeyBuffer();
    else a=pop();
    
    KeyCounter = 0; // clear buffer

    result=b-a;
    push(result);
  }


  if (button == '*')
  {
    double a,b;
    b=pop();
    if( KeyCounter > 0 ) a = pushKeyBuffer();
    else a=pop();
    
    KeyCounter = 0; // clear buffer

    result=b*a;
    push(result);
  }
  
  if (button == '/')
  {
    double a,b;
    b=pop();
    if( KeyCounter > 0 ) a = pushKeyBuffer();
    else a=pop();
    
    KeyCounter = 0; // clear buffer

    result=b/a;
    push(result);
  }
  
  if (button == 'c')
  {
    KeyCounter=0;
    NumberStackCounter=0;
    result=0;
    
  }
  
  tft.fillScreen( LTDC_BLACK );
  showKeyBuffer();
  tft.setCursor(0, 50);

  tft.print(result);

  delay(10);
}

