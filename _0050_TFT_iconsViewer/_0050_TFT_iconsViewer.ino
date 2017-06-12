/************************************************************************

  STM32F746 Discovery TFT example template

  Required libraries: GFX, Adafruit

  Installation Instructions:
  1. Import the Adadruit library in the menue
  Sketch=>Include Libraries=>Manage Libraries =>  Adafruit ILI9341
  2. Import the Adadruit GFX-Library
  Sketch=>Include Libraries=>Manage Libraries =>  Adafruit GFX Library


  June 2017, ChrisMicro

************************************************************************/

#include "LTDC_F746_Discovery.h"
#include "images.h"

LTDC_F746_Discovery tft;

void showImage(int x, int y)
{
  int w, h;
  uint16_t width,height,planeSize;

  width  = ImageDirectory[0].width;
  height = ImageDirectory[0].height;

  planeSize=width*height;
  

  
  for (h = 0; h < height; h++)
  {
    for (w = 0; w < width; w++)
    {
      int adr = w + h * width ;
      
      int r = images[ adr ];
      int g = images[ adr + planeSize ];
      int b = images[ adr + planeSize * 2];

      uint16_t color=tft.color565(r, g, b);

      /*
      tft.drawPixel(w * 2 + x, h * 2 + y, color );
      tft.drawPixel(w * 2 + x + 1, h * 2 + y, color);
      tft.drawPixel(w * 2 + x, h * 2 + 1 + y, color);
      tft.drawPixel(w * 2 + x + 1, h * 2 + 1 + y, color);
      */
      tft.drawPixel(w  + x, h + y, color );
    }
  }
  //delay(100);
  //tft.fillRect(x,y,width*2,height*2, LTDC_BLACK);

}


void setup()
{
  // The buffer is memory mapped
  // You can directly draw on the display by writing to the buffer
  uint16_t *buffer = (uint16_t *)malloc(LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height);

  tft.begin((uint16_t *)buffer);
  tft.fillScreen(LTDC_BLACK);
  //tft.setRotation(0);
  tft.setCursor(0, 100);
  tft.setTextColor(LTDC_GREEN);  tft.setTextSize(3);
  tft.println("STM32F746 Discovery");
  tft.setTextColor(LTDC_YELLOW); tft.setTextSize(2);

  showImage(200,10);

}

int Counter = 0;

void loop(void)
{
  //tft.print( Counter ); tft.print(" ");
  Counter ++;
  int x,y;
  x=(Counter*50)%470;
  y=(Counter*50)/470;
  
  showImage(x,y);
  delay(1000);


}

/*
 % matlab octave image to c-file converter
pkg load image
%image to byte array converter
a=imread("fly.png");
b=imresize(a,0.1);

# test cross image
if 0
  clear b
  # h=2, w=3
  b=zeros(2,3,3);
  b(1,:,1)=ones(3,1)*255; % horizontal line
  b(:,1,1)=ones(2,1)*128; % vertikal line
  
end

# octave matrix a(y,x) 
# x and y has changed order compared to usual cartessian
# coordinates
h=length(b(:,1,1));
w=length(b(1,:,1));
d=w*h;

printf("#define IMAGE_WIDTH %d\n\r",w);
printf("#define IMAGE_HEIGTH %d\n\r",h);
printf("#define PLANE_SIZE IMAGE_WIDTH*IMAGE_HEIGTH \n\r");

lineLength=32;
l=lineLength;
printf("const uint8_t images[]={\n\r");
for farbe=1:3,
  for hh=1:h,    
    for ww=1:w,
     printf("%d",b(hh,ww,farbe));
     if ww*hh*farbe~=w*h*3,
        printf(",");
     end
    end
    printf("\n\r");
  end

end

printf("\n\r};\n\r");
*/
 

