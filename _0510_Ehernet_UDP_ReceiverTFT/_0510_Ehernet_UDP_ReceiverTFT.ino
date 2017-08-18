/*
 STM32F746 Discovery UDP receiver.
 
 This sketch receives UDP message strings, prints them on the TFT

 A java sketch is included at the end of file that can be used to send 
 messages for testing with a computer.

 by ChrisMicro 18. Aug. 2017


 derived from UDPSendReceiveString 
 from Michael Margolis, 21. Aug. 2010
 
 This code is in the public domain.
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

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  

EthernetUDP Udp;

void setup() 
{
  // The buffer is memory mapped
  // You can directly draw on the display by writing to the buffer
  uint16_t *buffer = (uint16_t *)malloc(2*LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height);
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

void loop() 
{
  int packetSize = Udp.parsePacket();
  
  if ( packetSize )   // if there's data available, print it 
  {
    tft.fillScreen(LTDC_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(LTDC_GREEN);
    tft.print("Received packet of size ");    
    tft.println(packetSize);
    tft.print("From ");

    IPAddress fromIpAddress = Udp.remoteIP();
    for (int i = 0; i < 4; i++) 
    {
      tft.print(fromIpAddress[i], DEC);
      if (i < 3) tft.print(".");  
    }
    tft.print(", port ");
    tft.println(Udp.remotePort());

    int len = packetSize;
    if( len > UDP_TX_PACKET_MAX_SIZE-1 ) len = UDP_TX_PACKET_MAX_SIZE-1;
    
    Udp.read(packetBuffer, packetSize);    
    
    packetBuffer[len] = 0;// end of string
    tft.setTextColor(LTDC_YELLOW);
    tft.println(packetBuffer);

  }
  delay(10);
}



