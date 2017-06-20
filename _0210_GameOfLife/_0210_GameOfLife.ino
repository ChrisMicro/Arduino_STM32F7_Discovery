///////////////////////////////////////////////////////////////
//
// LIFE v0.1
// by Michele Pinassi <o-zone@zerozone.it>
//
// For Arduino UNO R3 + TFT Lcd ILI9313 + TouchScreen
//
// https://github.com/michelep/ArduinoLifeGame/blob/master/life.ino
//
///////////////////////////////////////////////////////////////
/*
  to be reworked ....

  ChrisMicro   June 2017
*/

#include "LTDC_F746_Discovery.h" // TFT
#include "TouchScreen_F7_Discovery.h" // TOUCH

LTDC_F746_Discovery tft;
TouchScreen         ts;

/*
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
*/
// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0xCCCC

#define MAX_X 450
#define MAX_Y 250

#define CELLSIZE 10
#define CELL_LIFE 100

#define WORLD_X MAX_X/CELLSIZE
#define WORLD_Y MAX_Y/CELLSIZE

// Array bidimensionale contenente i dati del "mondo"
uint8_t worldArray[WORLD_X][WORLD_Y]={0};

void drawCell(uint8_t x, uint8_t y, uint8_t color) {
  tft.fillRect(x * CELLSIZE + 1, y * CELLSIZE + 1, CELLSIZE - 2, CELLSIZE - 2, color);
}

void drawWorld(void) {
  uint16_t x, y;

  // Serial.println(F("draw world grid"));

  for (x = 0; x < MAX_X; x += CELLSIZE) {
    tft.drawFastVLine(x, 0, MAX_Y, GRAY);
  }

  for (y = 0; y < MAX_Y; y += CELLSIZE) {
    tft.drawFastHLine(0, y, MAX_X, GRAY);
  }

  tft.drawFastHLine(0, 280, 240, GRAY);
}

// RULES of Life
// Any live cell with fewer than two live neighbours dies, as if caused by under-population.
// Any live cell with two or three live neighbours lives on to the next generation.
// Any live cell with more than three live neighbours dies, as if by over-population.
// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

int8_t checkMatrix[16] {
  1, 1,
  1, 0,
  0, 1,
  -1, -1,
  -1, 0,
  0, -1,
  1, -1,
  -1, 1,
};

uint8_t cellNeighbours(uint16_t x, uint16_t y) {
  // Return the number of neighbours for the cell @ x,y
  uint8_t c, r = 0;
  int16_t rx, ry;

  for (c = 0; c < 16; c += 2) {
    rx = x + checkMatrix[c];
    ry = y + checkMatrix[c + 1];
    if (rx < 0) rx = MAX_X;
    if (ry < 0) ry = MAX_Y;
    if (rx > MAX_X) rx = 0;
    if (ry > MAX_Y) ry = 0;
    if (worldArray[rx][ry] > 0) r++;
  }
  return r;
}

uint16_t oldn = 255;

void updateWorld() {
  uint8_t x, y, c;
  uint16_t n = 0;
  for (x = 0; x < WORLD_X; x++) {
    for (y = 0; y < WORLD_Y; y++) {
      c = cellNeighbours(x, y);

      if (c == 3) worldArray[x][y] = CELL_LIFE; // Born == 3

      if (worldArray[x][y] > 0) {
        /*       if(c > 0) {
                 Serial.print(F("Cell "));
                 Serial.print(x,DEC);
                 Serial.print(":");
                 Serial.print(y,DEC);
                 Serial.print(F(" neigh "));
                 Serial.println(c,DEC);
               } */

        worldArray[x][y]--;
        if (worldArray[x][y] > 20) {
          if ((c > 1) && (c < 4)) worldArray[x][y]++; // Remain alive
          // Colora cella
          drawCell(x, y, WHITE);
          n++;
        } else if (worldArray[x][y] < 20) {
          drawCell(x, y, RED);
        }
      }
    }
  }
  if (n != oldn) 
  {
    tft.fillRect(50, 255, 240, 272-255, BLACK);
    tft.setCursor(5, 255);
    tft.setTextColor(GRAY);
    tft.setTextSize(2);
    tft.print("Life: ");
    tft.print(n, DEC);

    oldn = n;
    /* Calculate percentual of life in the world */
  }
}

void initCell(uint16_t cx, uint16_t cy) 
{
  uint8_t x, y;
  //Serial.print(F("Initialize cell "));
  for (x = 0; x < WORLD_X; x++) 
  {
    for (y = 0; y < WORLD_Y; y++) 
    {
      if ((cx > x * CELLSIZE) && (cx < x * CELLSIZE + CELLSIZE)) 
      {
        if ((cy > y * CELLSIZE) && (cy < y * CELLSIZE + CELLSIZE)) 
        {
          worldArray[x][y] = CELL_LIFE;
          /*
          Serial.print(x,DEC);
          Serial.print(":");
          Serial.println(y,DEC);
          delay(100);
          */
        }
      }
    }
  }
}

void setup(void) 
{
  Serial.begin(115200);
  // The buffer is memory mapped
  // You can directly draw on the display by writing to the buffer
  uint16_t *buffer = (uint16_t *)malloc(LTDC_F746_ROKOTECH.width * LTDC_F746_ROKOTECH.height);

  tft.begin((uint16_t *)buffer);

  // Pulisci lo schermo
  tft.fillScreen(BLACK);

  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);

  drawWorld();

  //pinMode(13, OUTPUT);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{
  uint8_t c;

  for (c = 0; c < 2; c++) 
  {
    //digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    //int16_t tempX = p.x;
    //p.x = 1023-p.y;
    //p.y = 1023-tempX;
    //digitalWrite(13, LOW);

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    {

      //p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      //p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
/*
      Serial.print("X = "); Serial.print(p.x);
      Serial.print("\tY = "); Serial.print(p.y);
      Serial.print("\tPressure = "); Serial.println(p.z); 
*/
      initCell(p.x, p.y);
      drawWorld();
    }else
    {
      if(c==0)
      {updateWorld();
      delay(100);}
    }
  }


}


