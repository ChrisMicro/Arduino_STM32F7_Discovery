/************************************************************************

  STM32F746 Discovery TFT UGUI test

  UGUI is a GUI graphic driver for mikrocontrollers which needs only a
  pixel drawing routine callback.

  It can be fould here:
  https://github.com/achimdoebler/UGUI
  
  Required libraries: GFX, Adafruit

  Installation Instructions:
  1. Import the Adadruit library in the menue
  Sketch=>Include Libraries=>Manage Libraries =>  Adafruit ILI9341
  2. Import the Adadruit GFX-Library
  Sketch=>Include Libraries=>Manage Libraries =>  Adafruit GFX Library

  The code is mainly from Achim Doebler and addapted to the 
  STM32F746 Environment.

  There are some issues with button reaction but I post this
  code just to view the optics. 
  
  June 2017, ChrisMicro

************************************************************************/
#include <Wire.h>
#include "stm32_ub_touch_480x272.h"
#include "LTDC_F746_Discovery.h"
#include "ugui.h"

LTDC_F746_Discovery tft;

UG_GUI gui;

#define MAX_OBJECTS        10

/* Window 1 */
UG_WINDOW window_1;
UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];
UG_BUTTON button1_1;
UG_BUTTON button1_2;
UG_BUTTON button1_3;
UG_BUTTON button1_4;
UG_BUTTON button1_5;
UG_BUTTON button1_6;

/* Window 2 */
UG_WINDOW window_2;
UG_OBJECT obj_buff_wnd_2[MAX_OBJECTS];
UG_BUTTON button2_1;
UG_TEXTBOX textbox2_1;
UG_TEXTBOX textbox2_2;
UG_IMAGE image2_1;

/* Window 3 */
UG_WINDOW window_3;
UG_OBJECT obj_buff_wnd_3[MAX_OBJECTS];
UG_BUTTON button3_1;
UG_TEXTBOX textbox3_1;

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

// this is the main pixel routine needed by UGUI
// The pointer to this function will be set
// for UGUI with UG_Init
void pset(UG_S16 x, UG_S16 y, UG_COLOR col)
{
  tft.drawPixel(x, y, col);
}

static TP_STATE* TP_State;
TP_STATE * IOE_TP_GetState()
{
  static TP_STATE thisState;

  UB_Touch_Read();

  thisState.X = Touch_Data.xp;
  thisState.Y = Touch_Data.yp;
  thisState.Z = 0;
  if (P_Touch_GetContacts())
  {
    thisState.TouchDetected = TOUCH_STATE_PRESSED;
  }
  else
  {
    thisState.TouchDetected = TOUCH_STATE_RELEASED;
  }
  return &thisState;
}

/* FSM */
#define STATE_MAIN_MENU                0
#define STATE_BENCHMARK_RUN            1
#define STATE_BENCHMARK_RESULT         2

volatile UG_U32 state = STATE_MAIN_MENU;
volatile UG_U32 next_state;

/* Benchmark */
volatile UG_U32 timer;
volatile UG_U32 hw_acc = 1;
char result_str[30];
UG_S16 xs, ys;
UG_S16 xe, ye;
UG_COLOR c;

void SysTick_Handler(void)
{

  if ( timer ) timer--;

  if ( state == STATE_MAIN_MENU )
  {
    TP_State = IOE_TP_GetState();
    if ( TP_State->TouchDetected )
    {
      if ( (TP_State->X > 0) && (TP_State->X < 480 ) )
      {
        if ( (TP_State->Y > 0) && (TP_State->Y < 272 ) )
        {
          UG_TouchUpdate(TP_State->X, TP_State->Y, TOUCH_STATE_PRESSED);
        }
      }
    }
    else
    {
      UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);
    }
  }

  UG_Update();
}

void window_1_callback( UG_MESSAGE* msg )
{
  if ( msg->type == MSG_TYPE_OBJECT )
  {
    if ( msg->id == OBJ_TYPE_BUTTON )
    {
      switch ( msg->sub_id )
      {
        case BTN_ID_0: /* Toggle green LED */
          {
            //TOGGLE_GREEN_LED;
            UG_WindowSetTitleColor( &window_1, C_GREEN );
            break;
          }
        case BTN_ID_1: /* Toggle red LED */
          {
            //TOGGLE_RED_LED;
            UG_WindowSetTitleColor( &window_1, C_RED );
            break;
          }
        case BTN_ID_2: /* Show µGUI info */
          {
            UG_WindowShow( &window_2 );
            break;
          }
        case BTN_ID_3: /* Toggle hardware acceleration */
          {
            if ( !hw_acc )
            {
              UG_ButtonSetForeColor( &window_1, BTN_ID_3, C_RED );
              UG_ButtonSetText( &window_1, BTN_ID_3, "HW_ACC\nOFF" );
              UG_DriverEnable( DRIVER_DRAW_LINE );
              UG_DriverEnable( DRIVER_FILL_FRAME );
            }
            else
            {
              UG_ButtonSetForeColor( &window_1, BTN_ID_3, C_BLUE );
              UG_ButtonSetText( &window_1, BTN_ID_3, "HW_ACC\nON" );
              UG_DriverDisable( DRIVER_DRAW_LINE );
              UG_DriverDisable( DRIVER_FILL_FRAME );
            }
            hw_acc = !hw_acc;
            break;
          }
        case BTN_ID_4: /* Start benchmark */
          {
            next_state = STATE_BENCHMARK_RUN;
            break;
          }
        case BTN_ID_5: /* Resize window */
          {
            static UG_U32 tog;

            if ( !tog )
            {
              UG_WindowResize( &window_1, 0, 40, 239, 319 - 40 );
            }
            else
            {
              UG_WindowResize( &window_1, 0, 0, 239, 319 );
            }
            tog = ! tog;
            break;
          }
      }
    }
  }
}

/* Callback function for the info window */
void window_2_callback( UG_MESSAGE* msg )
{
  if ( msg->type == MSG_TYPE_OBJECT )
  {
    if ( msg->id == OBJ_TYPE_BUTTON )
    {
      switch ( msg->sub_id )
      {
        case BTN_ID_0:
          {
            UG_WindowHide( &window_2 );
            break;
          }
      }
    }
  }
}

/* Callback function for the result window */
void window_3_callback( UG_MESSAGE* msg )
{
  if ( msg->type == MSG_TYPE_OBJECT )
  {
    if ( msg->id == OBJ_TYPE_BUTTON )
    {
      switch ( msg->sub_id )
      {
        /* OK button */
        case BTN_ID_0:
          {
            UG_WindowShow( &window_1 );
            break;
          }
      }
    }
  }
}



void setup()
{
  // init touch connected to I2C3
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
  tft.setTextColor(LTDC_GREEN);  tft.setTextSize(3);
  tft.println("STM32F746 Discovery");
  tft.setTextColor(LTDC_YELLOW); tft.setTextSize(2);

  delay(1000);


  //UG_S16 UG_Init( UG_GUI* g, void (*p)(UG_S16,UG_S16,UG_COLOR), UG_S16 x, UG_S16 y )
  UG_Init(&gui, (void(*)(UG_S16, UG_S16, UG_COLOR))pset, 480, 272);

  /* Register hardware acceleration */

  /* Clear Screen */
  UG_FillScreen( C_BLACK );

  /* Create Window 1 */
  UG_WindowCreate( &window_1, obj_buff_wnd_1, MAX_OBJECTS, window_1_callback );
  UG_WindowSetTitleText( &window_1, "µGUI @ STM32F746" );
  UG_WindowSetTitleTextFont( &window_1, &FONT_12X20 );


  UG_ButtonCreate( &window_1, &button1_1, BTN_ID_0, 10, 10, 110, 60 );
  UG_ButtonCreate( &window_1, &button1_2, BTN_ID_1, 10, 80, 110, 130 );
  UG_ButtonCreate( &window_1, &button1_3, BTN_ID_2, 10, 150, 110, 200 );
  UG_ButtonCreate( &window_1, &button1_4, BTN_ID_3, 120, 10, UG_WindowGetInnerWidth( &window_1 ) - 10 , 60 );
  UG_ButtonCreate( &window_1, &button1_5, BTN_ID_4, 120, 80, UG_WindowGetInnerWidth( &window_1 ) - 10, 130 );
  UG_ButtonCreate( &window_1, &button1_6, BTN_ID_5, 120, 150, UG_WindowGetInnerWidth( &window_1 ) - 10, 200 );


  UG_ButtonSetFont( &window_1, BTN_ID_0, &FONT_12X20 );
  UG_ButtonSetBackColor( &window_1, BTN_ID_0, C_LIME );
  UG_ButtonSetText( &window_1, BTN_ID_0, "Green\nLED" );

  UG_ButtonSetFont( &window_1, BTN_ID_1, &FONT_12X20 );
  UG_ButtonSetBackColor( &window_1, BTN_ID_1, C_RED );
  UG_ButtonSetText( &window_1, BTN_ID_1, "Red\nLED" );

  UG_ButtonSetFont( &window_1, BTN_ID_2, &FONT_12X20 );
  UG_ButtonSetText( &window_1, BTN_ID_2, "About\nµGUI" );
  UG_WindowShow( &window_1 );

  UG_ButtonSetFont( &window_1, BTN_ID_3, &FONT_12X20 );
  UG_ButtonSetForeColor( &window_1, BTN_ID_3, C_RED );
  UG_ButtonSetText( &window_1, BTN_ID_3, "HW_ACC\nOFF" );

  UG_ButtonSetFont( &window_1, BTN_ID_4, &FONT_8X14 );
  UG_ButtonSetText( &window_1, BTN_ID_4, "Start\nBenchmark" );

  UG_ButtonSetFont( &window_1, BTN_ID_5, &FONT_10X16 );
  UG_ButtonSetText( &window_1, BTN_ID_5, "Resize\nWindow" );

  /* -------------------------------------------------------------------------------- */
  /* Create Window 2 (µGUI Info)                                                      */
  /* -------------------------------------------------------------------------------- */
  UG_WindowCreate( &window_2, obj_buff_wnd_2, MAX_OBJECTS, window_2_callback );
  UG_WindowSetTitleText( &window_2, "Info" );
  UG_WindowSetTitleTextFont( &window_2, &FONT_12X20 );
  UG_WindowResize( &window_2, 20, 40, 219, 279 );

  /* Create Button 1 */
  UG_ButtonCreate( &window_2, &button2_1, BTN_ID_0, 40, 150, UG_WindowGetInnerWidth( &window_2 ) - 40, 200 );
  UG_ButtonSetFont( &window_2, BTN_ID_0, &FONT_22X36 );
  UG_ButtonSetText( &window_2, BTN_ID_0, "OK!" );

  /* Create Textbox 1 */
  UG_TextboxCreate( &window_2, &textbox2_1, TXB_ID_0, 10, 10, UG_WindowGetInnerWidth( &window_2 ) - 10, 40 );
  UG_TextboxSetFont( &window_2, TXB_ID_0, &FONT_16X26 );
  UG_TextboxSetText( &window_2, TXB_ID_0, "µGUI v0.3" );
  UG_TextboxSetAlignment( &window_2, TXB_ID_0, ALIGN_TOP_CENTER );

  /* Create Textbox 2 */
  UG_TextboxCreate( &window_2, &textbox2_2, TXB_ID_1, 10, 125, UG_WindowGetInnerWidth( &window_2 ) - 10, 135 );
  UG_TextboxSetFont( &window_2, TXB_ID_1, &FONT_6X8 );
  UG_TextboxSetText( &window_2, TXB_ID_1, "www.embeddedlightning.com" );
  UG_TextboxSetAlignment( &window_2, TXB_ID_1, ALIGN_BOTTOM_CENTER );
  UG_TextboxSetForeColor( &window_2, TXB_ID_1, C_BLUE );
  UG_TextboxSetHSpace( &window_2, TXB_ID_1, 1 );

  /* Create Image 1 */
  //UG_ImageCreate( &window_2, &image2_1, IMG_ID_0, (UG_WindowGetInnerWidth( &window_2 )>>1) - (logo.width>>1), 40, 0, 0 );
  //UG_ImageSetBMP( &window_2, IMG_ID_0, &logo );

  /* -------------------------------------------------------------------------------- */
  /* Create Window 3 (Benchmark Result)                                               */
  /* -------------------------------------------------------------------------------- */
  UG_WindowCreate( &window_3, obj_buff_wnd_3, MAX_OBJECTS, window_3_callback );
  UG_WindowSetTitleText( &window_3, "Benchmark Result" );
  UG_WindowSetTitleTextFont( &window_3, &FONT_10X16 );
  UG_WindowResize( &window_3, 20, 90, 219, 230 );

  /* Create Textbox 1 */
  UG_TextboxCreate( &window_3, &textbox3_1, TXB_ID_0, 5, 10, UG_WindowGetInnerWidth( &window_3 ) - 5, 60 );
  UG_TextboxSetFont( &window_3, TXB_ID_0, &FONT_12X20 );
  UG_TextboxSetText( &window_3, TXB_ID_0, "Result:\n99999 frm/sec" );
  UG_TextboxSetAlignment( &window_3, TXB_ID_0, ALIGN_TOP_CENTER );

  /* Create Button 1 */
  UG_ButtonCreate( &window_3, &button3_1, BTN_ID_0, 40, 65, UG_WindowGetInnerWidth( &window_3 ) - 40, 100 );
  UG_ButtonSetFont( &window_3, BTN_ID_0, &FONT_12X16 );
  UG_ButtonSetText( &window_3, BTN_ID_0, "OK!" );


  UG_WindowShow( &window_1 );
  UG_Update();
  delay(1000);
}

/* better rand() function */
UG_U32 randx( void )
{
   static UG_U32 z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
   UG_U32 b;
   b  = ((z1 << 6) ^ z1) >> 13;
   z1 = ((z1 & 4294967294U) << 18) ^ b;
   b  = ((z2 << 2) ^ z2) >> 27;
   z2 = ((z2 & 4294967288U) << 2) ^ b;
   b  = ((z3 << 13) ^ z3) >> 21;
   z3 = ((z3 & 4294967280U) << 7) ^ b;
   b  = ((z4 << 3) ^ z4) >> 12;
   z4 = ((z4 & 4294967168U) << 13) ^ b;
   return (z1 ^ z2 ^ z3 ^ z4);
}

int Counter = 0;

void loop(void)
{
  SysTick_Handler();
  delay(10);
  /*
  tft.fillRect(10, 250, 100, 20, C_BLACK);
  tft.setCursor(10, 250);
  tft.print( Counter );
  Counter ++;
  delay(100);*/

  static int frm_cnt;

  /* Do we change the state? */
  if ( next_state != state )
  {
    /* Initialize the next state */
    switch ( next_state )
    {
      case STATE_MAIN_MENU:
        {
          /* Nothing to do */
          break;
        }
      case STATE_BENCHMARK_RUN:
        {
          /* Clear layer 2 */
          //ltdc_draw_layer( LAYER_2 );
          UG_FillScreen( C_BLACK );

          /* Fade to layer 2 */
          //ltdc_fade_to_layer( LAYER_2 );

          /* Reset the frame counter */
          frm_cnt = 0;

          /* Run benchmark for 5 seconds */
          timer = 500;
          break;
        }
      case STATE_BENCHMARK_RESULT:
        {
          /* Nothing to do */
          break;
        }
    }
    state = next_state;
  }

  /* FSM */
  switch ( state )
  {
    /* Run the benchmark */
    case STATE_BENCHMARK_RUN:
      {
        xs = randx() % 480;
        xe = randx() % 480;
        ys = randx() % 272;
        ye = randx() % 272;
        c = randx() % 0xFFFFFF;
        UG_FillFrame( xs, ys, xe, ye, c );
        frm_cnt++;

        if ( !timer ) next_state = STATE_BENCHMARK_RESULT;
        break;
      }
    /* Show benchmark result */
    case STATE_BENCHMARK_RESULT:
      {
        sprintf( result_str, "Result:\n%u frm/sec", frm_cnt / 5 );
        UG_TextboxSetText( &window_3, TXB_ID_0, result_str );

        /* Fade to layer 1 */
        //ltdc_draw_layer( LAYER_1 );
        //ltdc_fade_to_layer( LAYER_1 );

        /* Show benchmark result */
        UG_WindowShow( &window_3 );

        next_state = STATE_MAIN_MENU;
        break;
      }
    case STATE_MAIN_MENU:
      {
        /* Let µGUI do the job! */
        break;
      }
  }

}


