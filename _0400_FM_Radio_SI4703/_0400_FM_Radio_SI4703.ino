/*


  STM32F746 FM-radio using SI4703 chip and GuiPittix

   https://learn.sparkfun.com/tutorials/si4703-fm-radio-receiver-hookup-guide

   required libraries
   https://cdn.sparkfun.com/assets/learn_tutorials/2/7/4/Si4703_Breakout.zip

   GUI-Library
   https://github.com/ChrisMicro/GuiPittix

   Hardware:
   STM32F746 Discovery
   Sparkfun SI4703 receiver breakout board

   connections:
   - GND
   - 3.3V
   - SDIO            PB9 // Arduino header pin D14 => prolounged header see F746 disco data sheet
   - SCLK            PB8 // Arduino header pin D15 
   - SI4703 resetPin PI2 // Arduino header pin D8


   2017 ChrisMicro

*/

#define TFT_Adapter_STM32F746_Discovery
#include "TouchScreen_F7_Discovery.h" // TOUCH

#include <Si4703_Breakout.h>
//#include <Wire.h>
#include "Gui.h"

int resetPin = PI2;
int SDIO = PB9; // D14
int SCLK = PB8; // D15

Si4703_Breakout radio(resetPin, SDIO, SCLK);

TouchScreen ts;

GUI_Slider sliderVolume ("vol"  );
GUI_labeledObject   textSeek("seek");
GUI_Button buttonSeekUp("up");
GUI_Button buttonSeekDown("down");
GUI_Number numberChannel("channel");

GUI_Text textRDS("RDS");

GUI_Led led(350,220,"alive");

int channel;
int oldVolume=0;

char rdsBuffer[10]={0};

void setup()
{
  radio.powerOn();
  radio.setVolume(10);
  
  gui.begin();
  sliderVolume.show();
  textSeek.showLabel();
  buttonSeekUp.show();
  buttonSeekDown.show();
  numberChannel.show();
  //textRDS.show();
  
  textRDS.print(rdsBuffer);
 
  channel=903;
  radio.setChannel(channel);
  numberChannel.print(channel);
  
}


void loop()
{
  // read touch pad and tell the gui
  TSPoint p = ts.getPoint();
  gui.setTouch(p.x, p.y, p.z);

  int vol = sliderVolume.getInt() / 6;
  if(vol!=oldVolume)
  {
    oldVolume=vol;
    radio.setVolume(vol);
  }

  if (buttonSeekUp.wasPressed())
  {
    channel = radio.seekUp();
    numberChannel.print(channel);
  }

  if (buttonSeekDown.wasPressed())
  {
    channel = radio.seekDown();
    numberChannel.print(channel);
  }
  /*
if(led.isHit())
{
   radio.readRDS(rdsBuffer, 15000);  
   strcpy(rdsBuffer,"none");
   textRDS.print(rdsBuffer);
}*/

  led.toggle();

  delay(50);

}
