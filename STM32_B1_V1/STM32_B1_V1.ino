/* 9/9/2021
  Written by B J Mclaren, Feel free to ammend or PM if major problem

  GOTTLIEB EM Pinball Controller using the COBRAPIN Pinball Controller
  with the STM32DUINO IDE loaded (NOT OPP/MPF compatible)
  replacing the EM Stuff and LEDS replacing bulbs.

  Based on experience of building an ARDUINO IDE based pinball controller
  using Arduino MEGA and UNO's with FREETRONICS MOSFET Shields
  This is Board 1 and processes all the switches and is the Master
  Controller for the game.
  NOTE Board 1 and 0 communicate via a serial connection NOT a PC
  like in MPF solution
  Board 0 controls the Pop Bumpers, Score Reels, Ball Drain and Chimes
  Board 2 controls the Flippers
*/
#include "pins.h"
#include <FastLED.h>
#include <SoftSerial.h>
SoftSerial mySerial(PC14, PC15, 3);
// Special LED offsets
int R_Special_LED1 = 23;
int R_Special_LED2 = 56;
int L_Special_LED1 = 16;
int L_Special_LED2 = 26;
// Start of Balls Blayed LEDs
CRGB leds[NUM_LEDS];
int FCLamps [13] =
{
  21, 22, 24, 25, 27, 28, 34, 33, 32, 31, 30, 29, 0
};
// switch LED's on Playfield
int FCswitchLamps [13] =
{
  17, 8, 57, 5, 35, 50, 54, 6, 13, 7, 49, 36, 0
};
// Target LED's on Playfield
int FCTargetLamps [13] =
{
  59, 20, 9, 51, 11, 60, 12, 52, 14, 19, 61, 10, 0
};
// 12 slots to indicate "FLYING CARPET" Targets/Rollovers hit
int FC[13];                     // slots 0 and 13 not used
int Balls = 0;
int RandNumber = 0;             // Lucky Matching number
int BallsPerGame = 5;
// Game Over has the Following Values
// 0 - Game ON; 1 - Game Over; 2 Game Over & Attract Mode; 3 - Fault
int Game_Over = 1;
int FCcount = 0;                // count of numbers of FC lights ON
int SpecialON = 0;              // Special ON
int BlueBumper = 0;             // Blue Score 100 if set to 1
int Phit = 0;                   // set if P rollover or target hit
int Ahit = 0;                   // set if A rollover or target hit
// if both Phit and Ahit hit: Blue bumper score 100
int Yspecial = 0;
int Rspecial = 0;
int replays = 0;
int units = 0;
int switches[No_of_Switches] =
{
  0, Fswitch, Lswitch, Yswitch, Iswitch, Nswitch, Gswitch,
  //
  Cswitch, Aswitch, Rswitch, Pswitch, Eswitch, Tswitch,
  Slingshot, Tilt
};
int Activated_Switch = 0;
int ButtonState[No_of_Switches] =
{
  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};
unsigned long diff = 0;
unsigned long last_on_time = 0;
unsigned long System_Base_Time = 0;
unsigned long previousmillis = 0;
int read_switch = 0;
//
void setup()
{
  //
  pinMode(BlueLed_Driver, OUTPUT);
  digitalWrite (BlueLed_Driver, LOW);
  pinMode (Flipper_Control , OUTPUT);
  digitalWrite(Flipper_Control , LOW);
  // LEDS
  pinMode(LED, OUTPUT);
  digitalWrite (LED, LOW);
  FastLED.addLeds<WS2811, DATA_PIN,  BRG>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  FastLED.show();
  // Switches and Targets
  pinMode(Fswitch, INPUT_PULLUP);
  pinMode(Lswitch, INPUT_PULLUP);
  pinMode(Yswitch, INPUT_PULLUP);
  pinMode(Iswitch, INPUT_PULLUP);
  pinMode(Nswitch, INPUT_PULLUP);
  pinMode(Gswitch, INPUT_PULLUP);
  //
  pinMode(Cswitch, INPUT_PULLUP);
  pinMode(Aswitch, INPUT_PULLUP);
  pinMode(Rswitch, INPUT_PULLUP);
  pinMode(Pswitch, INPUT_PULLUP);
  pinMode(Eswitch, INPUT_PULLUP);
  pinMode(Tswitch, INPUT_PULLUP);
  //
  pinMode(Slingshot, INPUT_PULLUP);
  pinMode(Tilt, INPUT_PULLUP);
  // Game Start Stuff
  pinMode(Startbutton, INPUT_PULLUP);
  pinMode(Drain_Switch, INPUT_PULLUP);
  //
  Serial.begin   (9600);
  delay(1000);
  mySerial.begin(9600);
  delay (1000);
  last_on_time = millis();
}
//
void Lucky()
// Are we Lucky today?
// match number for replay
// For Fun Only as we are a Free Play Machine
{
  int u = units - (int(units / 10) * 10);
  int r = random (0, 9);
  if (r == u)
  {
    replays++;
    mySerial.print(replays);
    mySerial.print('K');
  }
}//
void check_for_special(int letter)
// completing F-L-Y-I-N-G C-A-R-P-E-T sequence lights
// 'Y' Target
// switch alternatively to
// 'R' Target
// and back
{
  FC [letter] = 1;
  int Lmp = FCLamps[letter];
  leds[Lmp] = CRGB::White;
  int Tgt = FCTargetLamps[letter];
  leds[Tgt] = CRGB::Black;
  int roll = FCswitchLamps[letter];
  leds[roll] = CRGB::Black;
  FastLED.show();
  //check if special to be switched ON
  FCcount = 0;
  for (int i = 0; i < 12; i++)
  {
    if (FC[i] == 1)
    {
      FCcount = FCcount + 1;
    }
  }
  // check for specials in LED Array
  // and light special if 'Flying Carpet' fully lit
  if (FCcount == 12)
  {
    digitalWrite (BlueLed_Driver, HIGH);
    SpecialON = !SpecialON;
    if (SpecialON == 0 )
    {
      Rspecial = 1;
      Yspecial = 0;
      leds[R_Special_LED1] = CRGB::White;
      leds[R_Special_LED2] = CRGB::White;
      leds[L_Special_LED1] = CRGB::Black;
      leds[L_Special_LED2] = CRGB::Black;
      FastLED.show();
    }
    if (SpecialON == 1 )
    {
      Yspecial = 1;
      Rspecial = 0;
      leds[R_Special_LED1] = CRGB::Black;
      leds[R_Special_LED2] = CRGB::Black;
      leds[L_Special_LED1] = CRGB::White;
      leds[L_Special_LED2] = CRGB::White;
      FastLED.show();
    }
  }
}
void scan_switch_on()
{
  for (int i = 1; i < No_of_Switches; i++)
  {
    read_switch = digitalRead(switches[i]);
    if ((read_switch == 0) && (ButtonState[i] == 0))
    {
      digitalWrite (LED, HIGH);
      ButtonState[i] = 1;
      Activated_Switch = i;
      last_on_time = millis();
    }
  }
}
void scan_switch_off()
{
  // this is entered to see if any switchs have changed state
  // to OFF. It is a scheduled event.
  for (int i = 1; i < No_of_Switches; i++)
  {
    read_switch = digitalRead(switches[i]);
    if ((read_switch == 1) && (ButtonState[i] == 1))
    {
      digitalWrite (LED, LOW);
      ButtonState[i] = 0;
    }
  }
}
//
void Score50()
{
  mySerial.print('B');          // score 10 and Chime
  // repeat 5 times for a score of 50
  for (int i = 0; i < 5; i++)
  {
    leds [ i ] = CRGB::Black;
    FastLED.show();
    delay (50);
    if (i < 4 )mySerial.print ('F');          // score 40 No Chime
    leds [ i ] = CRGB::White;
    FastLED.show();
  }
}
//
void reset_sys()
{
  // Light all the LEDS
  for (int i = 0; i < 70; i = i + 1)
  {
    leds[i] = CRGB::White;
  }
  FastLED.show();
  // switch off LEDs that are spare underneath playfield
  leds [58] = CRGB::Black;
  leds [9] =  CRGB::Black;
  //
  FastLED.show();
  // switch of 4 special lights
  leds [R_Special_LED1] = CRGB::Black;
  leds [R_Special_LED2] = CRGB::Black;
  leds [L_Special_LED1] = CRGB::Black;
  leds [L_Special_LED2] = CRGB::Black;
  // now switch OFF central FLYING CARPET Lights
  for (int i = 0; i < 13; i = i + 1)
  {
    int FC_Lights = FCLamps[i];
    leds[FC_Lights] = CRGB::Black;
  }
  // Switch off Ball in play and Game Over LEDS
  for (int i = 0; i < 6; i = i + 1)
  {
    leds[i + 41] = CRGB::Black;
  }
  FastLED.show();
  digitalWrite(BlueLed_Driver, LOW);
  digitalWrite(BlueLed_Driver, LOW);
  BallsPerGame = 5;
  Balls = 0;
  units = 0;
  FCcount = 0;
  Phit = 0;
  Ahit = 0;
  Yspecial = 0;
  Rspecial = 0;
  Game_Over = 1;
  SpecialON = 0;               // = 1 if 'Y' target and -1 if R target
  BlueBumper = 0;              // = 1 to light; set to 2 when alight
  for (int i = 0; i < 12; i++)
  {
    FC[i] = 0;
  }
}
//
void ATTRACT1()
{
  // Light all the LEDS
  for (int i = 0; i <  NUM_LEDS; i++)
  {
    leds[i] = CRGB::White;
    delay (25);
    FastLED.show();
  }
  delay (50);
  // all LEDS OFF
  for (int i = 0; i <  NUM_LEDS; i++)
  {
    leds [ i ] = CRGB::Black;
    delay (25);
    FastLED.show();
  }
}
//
void Score()
{
  if (Activated_Switch > 0 )
  {
    switch (Activated_Switch)
    {
      case 0:               // Should never get here
        Activated_Switch = 0;
        break;
      case 1:               // F
        check_for_special(0);
        Score50();
        Activated_Switch = 0;
        break;
      case 2:               // L
        check_for_special(1);
        Score50();
        Activated_Switch = 0;
        break;
      case 3:               // Y
        check_for_special(2);
        Score50();
        Activated_Switch = 0;
        if (Yspecial == 1 )
        {
          // although Replays are awarded, they are for
          //              AMUSEMENT ONLY
          // The display software will ingores replays above 9
          replays++;
          mySerial.print(replays);
          mySerial.print('K');
        }
        break;
      case 4:               // I
        check_for_special(3);
        Score50();
        Activated_Switch = 0;
        break;
      case 5:               // N
        check_for_special(4);
        Score50();
        Activated_Switch = 0;
        break;
      case 6:               // G
        check_for_special(5);
        Score50();
        Activated_Switch = 0;
        break;
      case 7:               // C
        check_for_special(6);
        mySerial.print('C');
        Activated_Switch = 0;
        break;
      case 8:               // A
        check_for_special(7);
        mySerial.print('C');
        Ahit = 1;
        Activated_Switch = 0;
        break;
      case 9:               // R
        check_for_special(8);
        mySerial.print('C');
        Activated_Switch = 0;
        if (Rspecial == 1 )
        {
          // although Replays are awarded, they are for
          //              AMUSEMENT ONLY
          replays++;
          mySerial.print(replays);
          mySerial.print('K');
        }
        break;
      case 10:              // P
        check_for_special(9);
        mySerial.print('C');
        Phit = 1;
        Activated_Switch = 0;
        break;
      case 11:              // E
        check_for_special(10);
        mySerial.print('C');
        Activated_Switch = 0;
        break;
      case 12:              // T
        check_for_special(11);
        mySerial.print('C');
        Activated_Switch = 0;
        break;
      case 13:              // Slingshot
        mySerial.print('A');
        Activated_Switch = 0;
        break;
      case 14:              // Tilt
        mySerial.print ('O');
        Activated_Switch = 0;
        Game_Over = 1;
        break;
    }
  }
}
//
void diag()
{
  // check if any switches or target closed
  scan_switch_on();
  if (Activated_Switch > 0)
  {
    int tmp;
    tmp = Activated_Switch;
    tmp = tmp - 1;
    //int Lmp = FCLamps[tmp];
    //leds[Lmp] = CRGB::White;
    int Tgt = FCTargetLamps[tmp];
    leds[Tgt] = CRGB::White;
    int roll = FCswitchLamps[tmp];
    leds[roll] = CRGB::White;
    FastLED.show();
    Activated_Switch = 0;
    Game_Over = 3;              // Faulty Switch or Target
  }
}
//
void Start_Button()
{
  if (digitalRead(Startbutton) == 0 )
  {
    while (digitalRead(Startbutton) == 0 )
    {
      delay (25);
    }
    // wait till button released
    digitalWrite(Flipper_Control, HIGH);      // Enable Flippers
    delay (100);
    reset_sys();
    mySerial.print ('R');
    // Deliver 1st ball ready for new game start
    Balls = 1;
    leds[46] = CRGB::White;
    mySerial.print('N');
    for (int i = 0; i < 5; i = i + 1)
    {
      leds [ i ] = CRGB::Black;
      FastLED.show();
      delay (50);
      //
      leds [ i ] = CRGB::White;
      FastLED.show();
    }
    delay (2000);
    // replace delay later with a check to see if the ball
    // has passed the Ball in Lane Switch
    Game_Over = 0;
    digitalWrite (LED, HIGH);
  }
}
//
void loop()
{
  System_Base_Time = millis();
  Start_Button();
  if (Game_Over == 0 )
  {
    // Game ON so check switches and update score if necessary
    if ((Phit == 1) && (Ahit == 1))
    {
      if (BlueBumper == 0)
      {
        BlueBumper = 1;
        digitalWrite (BlueLed_Driver, HIGH);
        mySerial.print ('L');
      }
    }
    diff =  System_Base_Time - last_on_time;
    // Check if switches are now OFF
    if (diff > 200 )scan_switch_off();
    scan_switch_on();
    Score();
    if (digitalRead(Drain_Switch) == 0 )
    {
      delay (500);             // Let switch settle
      leds[47 - Balls] = CRGB::Black;
      Balls++;
      leds[47 - Balls] = CRGB::White;
      // If Game Over so tell backglass
      if (Balls > BallsPerGame)
      {
        Game_Over = 1;
      }
      else
      {
        for (int i = 0; i < 5; i = i + 1)
        {
          leds [ i ] = CRGB::Black;
          FastLED.show();
          delay (50);
          leds [ i ] = CRGB::White;
          FastLED.show();
        }
        // activate solonoid to kick ball into shooter lane
        mySerial.print('N');
      }
    }
  }
  if (Game_Over == 1)
    // Game Over light ON all others OFF
    // set Gameover to 2 since we only want it to
    // set the lights once
  {
    mySerial.print('O');
    digitalWrite(Flipper_Control, LOW);      // disable Flippers
    for (int i = 0; i < 70; i = i + 1)
    {
      leds [ i ] = CRGB::Black;
    }
    leds [ 41 ] = CRGB::White;
    FastLED.show();
    Game_Over = 2;
    digitalWrite (LED, LOW);
  }
  if (Game_Over == 2)
  { int alt_attract = 0;
    digitalWrite (LED, HIGH);
    diff =  System_Base_Time - previousmillis;
    if (diff > 6000)
    {
      digitalWrite (LED, LOW);
      ATTRACT1();
      diag();
      previousmillis = System_Base_Time;
    }
  }
}
