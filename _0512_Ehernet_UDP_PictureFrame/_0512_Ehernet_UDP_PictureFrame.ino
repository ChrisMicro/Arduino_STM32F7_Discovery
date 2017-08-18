/*
 STM32F746 Discovery UDP receiver.
 
 UDP picture frame receiver.

 by ChrisMicro 18. Aug. 2017
 
 */

#include <LwIP.h>
#include <STM32Ethernet.h>
#include <EthernetUdp.h>         

#include "LTDC_F746_Discovery.h"

LTDC_F746_Discovery tft;

byte mac[] = 
{
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(192, 168, 178, 177);

unsigned int localPort = 8888;      // local port to listen on

char packetBuffer[ UDP_TX_PACKET_MAX_SIZE ];  

EthernetUDP Udp;

uint16_t *buffer;
  
void setup() 
{
  // The buffer is memory mapped
  // You can directly draw on the display by writing to the buffer
  
  buffer = (uint16_t *) malloc( 2 * LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height );
  
  tft.begin((uint16_t *)buffer);
  tft.fillScreen(LTDC_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(LTDC_GREEN);  tft.setTextSize(3);
  tft.println("STM32F746 Disco UDP receiver");
  tft.setTextColor(LTDC_YELLOW); tft.setTextSize(2);

  // start the Ethernet and UDP:
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);

  tft.print("listening on port ");
  tft.println(localPort);
}

uint16_t widthIndex=100;
uint16_t heightIndex=100;

#define COMMAND_DRAWPIXEL   0
#define COMMAND_SETPOSITION 1

void loop() 
{
  int packetSize = Udp.parsePacket();
  
  if ( packetSize )   // if there's data available, print it 
  {
    
    int len = packetSize;
    //if( len > UDP_TX_PACKET_MAX_SIZE-1 ) len = UDP_TX_PACKET_MAX_SIZE-1;
    
    Udp.read(packetBuffer, packetSize);

    if(packetBuffer[0]==COMMAND_SETPOSITION)
    {
        widthIndex=packetBuffer[2];
        widthIndex+=packetBuffer[3]<<8;  
        heightIndex=packetBuffer[4];
        heightIndex+=packetBuffer[5]<<8;      
        //tft.println(widthIndex);
    }
    if(packetBuffer[0]==COMMAND_DRAWPIXEL)
    {
      uint32_t index=2;
    
      while(index<packetSize)
      {     
        uint16_t value=packetBuffer[index++];
        value+=packetBuffer[index++]<<8;
        //tft.setCursor(200,200);
        //tft.println(value,HEX);

        if( widthIndex < LTDC_F746_ROKOTECH.width ) tft.drawPixel(widthIndex,heightIndex,value);
        widthIndex++;

      }
    }

  }
  delay(10);
}
        


