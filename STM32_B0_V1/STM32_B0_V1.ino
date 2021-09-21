/* 9/9/2021
  Written by B J Mclaren, Feel free to ammend or PM if major problem

  GOTTLIEB EM Pinball Controller using the COBRAPIN Pinball Controller
  with the STM32DUINO IDE loaded (NOT OPP/MPF compatible)
  replacing the EM Stuff and LEDS replacing bulbs.

  Based on experience of building an ARDUINO IDE based Bally Bingo

  New Pin assignments to match COBRAPIN Controller

  Socket J7 for 3 chimes, 4 score reels
  Socket J6 for 3 bumpers, Ball Drain

  Board utilises a 13.5 Volt PSU as the coils are from a Gottlieb
  1970's EM machine which had an unregulated 24 Volt PSU
  giving approx 17 volts RMS when loaded so PSU set at approx 15 volt.

  If a 25 volt PSU is used then low PWM settings need to be used
  to stop balls hitting the glass or destroying targets

  Codes are  R = Reset & New Game
             O = Game Over
             N = Next Ball
             A = 1 point
             B = 10 points
             C = 100 points
             F = 10 points No Chime
             K = Knocker
             L = Pop Bumpers = 100 points

  NOTE PC14 and PC15 are reserved on both boards for inter board
  communications
  in addition I output on the USB port for 2 reasons 1) Debugging
  and 2) if an LCD/LED is connected to process sound and scoring
  using the Processing IDE on a PC
*/
#include <SoftSerial.h>
SoftSerial mySerial(PC14, PC15, 3);
//
// A total of 7 switches on J1 are available
// and a total of 5 on J2 plus SCK if required
// J1 Sockets Switches
#define B1_Switch  PB12   //0-0-1
#define B2_Switch  PB13   //0-0-2
#define B3_Switch  PB14   //0-0-3
// predefined spare switches TBD
#define S1_Switch  PB5    //0-0-11
#define S2_Switch  PB4    //0-0-10
#define S3_Switch  PB3    //0-0-9
#define S4_Switch  PA15   //0-0-8
//
// On J7 Socket MOSFETS
//
#define S1_Driver  PA8    //0-0-1
#define S2_Driver  PB7    //0-0-5
#define S3_Driver  PB8    //0-0-6
#define S4_Driver  PB9    //0-0-7
// Chime 1 - 3
#define C1_Driver  PA9    //0-0-2
#define C2_Driver  PA10   //0-0-3
#define C3_Driver  PB6    //0-0-4
// On J6 Socket
// Pop Bumpers 1 - 3 and Ball Drain
#define B1_Driver   PA0   //0-0-8
#define B2_Driver   PA1   //0-0-9
#define B3_Driver   PA2   //0-0-10
#define BD_Driver   PA3   //0-0-11
#define KN_Driver   PB0   //0-0-12
//
//#define LED PC13       // On Board LED OLD Style Jumpers
#define LED PB2          // On Board LED NEW Style Buttons
// Timers
unsigned long Bumper_Time = 50;
unsigned long Chime_Time  = 50;
unsigned long S_Time      = 50;
unsigned long BD_Time     = 50;
unsigned long KN_Time     = 50;
unsigned long SL_Time     = 50;
// Stops Pop Bumper Switch Bounce - set to match
// pinball
unsigned long Wait_Time = 250;
unsigned long Stuck_Time = 100;
//
int Coil_PWM_100 = 255;    // Max Power
int Coil_PWM_90 =  230;    // Medium Power
int Coil_PWM_75 =  190;    // Low Power
int Coil_PWM_60 =  150;    // Power for A1496 coils with 25 Volts
//

// Flags to indicate has been activated
int B1_ON = 0 ;           // Pop Bumpers
int B2_ON = 0 ;
int B3_ON = 0 ;
int C1_ON = 0;
int C2_ON = 0;
int C3_ON = 0;
int S1_ON = 0;            // Score Reels
int S2_ON = 0;
int S3_ON = 0;
int S4_ON = 0;
int BD_ON = 0;
int KN_ON = 0;
//
char ch1;
int Bumper100 = 0;
// Timers
unsigned long System_Base_Time = 0;
// Time when Coil last activated
unsigned long BD_ON_Time = 0;
unsigned long KN_ON_Time = 0;
unsigned long B1_ON_Time = 0;
unsigned long B2_ON_Time = 0;
unsigned long B3_ON_Time = 0;
unsigned long C1_ON_Time = 0;
unsigned long C2_ON_Time = 0;
unsigned long C3_ON_Time = 0;
unsigned long S1_ON_Time = 0;
unsigned long S2_ON_Time = 0;
unsigned long S3_ON_Time = 0;
unsigned long S4_ON_Time = 0;
unsigned long BD_diff = 0;
unsigned long KN_diff = 0;
unsigned long B1_diff = 0;
unsigned long B2_diff = 0;
unsigned long B3_diff = 0;
unsigned long C1_diff = 0;
unsigned long C2_diff = 0;
unsigned long C3_diff = 0;
unsigned long S1_diff = 0;
unsigned long S2_diff = 0;
unsigned long S3_diff = 0;
unsigned long S4_diff = 0;
//
// Reel Scoring Stuff
int S4 = 0;
int Old_S4 = 0;
int S3 = 0;
int S2 = 0;
int S1 = 0;
int NewScore = 0;
int AddScore = 0;
int GameOver = 1;
// Highest Score so far today
int HighScore = 0;
//
void setup()
{
  //
  pinMode(B1_Driver, OUTPUT);
  analogWrite(B1_Driver, 0);
  pinMode(B2_Driver, OUTPUT);
  analogWrite(B2_Driver, 0);
  pinMode(B3_Driver, OUTPUT);
  analogWrite(B3_Driver, 0);
  pinMode(C1_Driver, OUTPUT);
  analogWrite(C1_Driver, 0);
  pinMode(C2_Driver, OUTPUT);
  analogWrite(C2_Driver, 0);
  pinMode(C3_Driver, OUTPUT);
  analogWrite(C3_Driver, 0);
  pinMode(S1_Driver, OUTPUT);
  analogWrite(S1_Driver, 0);
  pinMode(S2_Driver, OUTPUT);
  analogWrite(S2_Driver, 0);
  pinMode(S3_Driver, OUTPUT);
  analogWrite(S3_Driver, 0);
  pinMode(S4_Driver, OUTPUT);
  analogWrite(S4_Driver, 0);
  pinMode(BD_Driver, OUTPUT);
  analogWrite(BD_Driver, 0);
  pinMode(KN_Driver, OUTPUT);
  analogWrite(KN_Driver, 0);
  // Score Reel / Pop Bumper / Slingshot  switches
  pinMode ( B1_Switch, INPUT_PULLUP);
  pinMode ( B2_Switch, INPUT_PULLUP);
  pinMode ( B3_Switch, INPUT_PULLUP);
  // Spare Switches
  pinMode ( S1_Switch, INPUT_PULLUP);
  pinMode ( S2_Switch, INPUT_PULLUP);
  pinMode ( S3_Switch, INPUT_PULLUP);
  pinMode ( S4_Switch, INPUT_PULLUP);
  // LED
  pinMode( LED, OUTPUT);
  digitalWrite(LED, LOW);
  //
  Serial.begin(9600);
  delay (1000);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  delay (1000);
  S4 = 0;
  Old_S4 = 0;
  S3 = 0;
  S2 = 0;
  S1 = 0;
  NewScore = 0;
  AddScore = 0;
  GameOver = 1;
  Bumper100 = 0;
}

//
void Split()
{
  S4 = int(NewScore / 1000);
  S3 = int(NewScore / 100);
  S3 = S3 - (int(S3 / 10) * 10);
  S2 = int(NewScore / 10);
  S2 = S2 - (int(S2 / 10) * 10);
  S1 = NewScore - (int(NewScore / 10) * 10);
}
void ResetScore()
{
  //
  if (S1 != 0)
  {
    S1++;
    analogWrite(S1_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
    S1_ON = 1;
    S1_ON_Time = millis();
    if (S1 == 10) S1 = 0;
  }
  //
  if (S2 != 0)
  {
    S2++;
    analogWrite(S2_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
    S2_ON = 1;
    S2_ON_Time = millis();
    if (S2 == 10) S2 = 0;
  }
  //
  if (S3 != 0)
  {
    S3++;
    analogWrite(S3_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
    S3_ON = 1;
    S3_ON_Time = millis();
    if (S3 == 10) S3 = 0;
  }
  if (S4 != 0)
  {
    S4++;
    analogWrite(S4_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
    S4_ON = 1;
    S4_ON_Time = millis();
    if (S4 == 10) S4 = 0;
  }
  // Check if Reels are really at 0000 by checking zero switch
  // on each reel in turn
  NewScore = 0;
  AddScore = 0;
}
//
void All_Coils_OFF()
{
  // called when faulty coil detected, switch all coils
  // to stop burnout
  analogWrite (BD_Driver, 0);
  analogWrite (KN_Driver, 0);
  analogWrite (B1_Driver, 0);
  analogWrite (B2_Driver, 0);
  analogWrite (B3_Driver, 0);
  analogWrite (S1_Driver, 0);
  analogWrite (S2_Driver, 0);
  analogWrite (S3_Driver, 0);
  analogWrite (S4_Driver, 0);
  analogWrite (C1_Driver, 0);
  analogWrite (C2_Driver, 0);
  analogWrite (C3_Driver, 0);
}
void Switch_Coils_OFF()
{
  if ( BD_ON == 1 )
  {
    BD_diff =  System_Base_Time -  BD_ON_Time;
    if ( BD_diff > Bumper_Time)
    {
      BD_ON = 0;
      analogWrite (BD_Driver, 0);
    }
  }
  if ( KN_ON == 1 )
  {
    KN_diff =  System_Base_Time -  KN_ON_Time;
    if ( KN_diff > KN_Time)
    {
      KN_ON = 0;
      analogWrite (KN_Driver, 0);
    }
  }
  if ( B1_ON == 1 )
  {
    B1_diff =  System_Base_Time -  B1_ON_Time;
    if ( B1_diff > Bumper_Time)
    {
      B1_ON = 0;
      analogWrite (B1_Driver, 0);
    }
  }

  if ( B2_ON == 1 )
  {
    B2_diff =  System_Base_Time -  B2_ON_Time;
    if ( B2_diff > Bumper_Time)
    {
      B2_ON = 0;
      analogWrite (B2_Driver, 0);
    }
  }
  if ( B3_ON == 1 )
  {
    B3_diff =  System_Base_Time -  B3_ON_Time;
    if ( B3_diff > Bumper_Time)
    {
      B3_ON = 0;
      analogWrite (B3_Driver, 0);
    }
  }
  if ( S1_ON == 1 )
  {
    S1_diff =  System_Base_Time -  S1_ON_Time;
    if (S1_diff > S_Time)
    {
      S1_ON = 0;
      analogWrite (S1_Driver, 0);
    }
  }
  if ( S2_ON == 1 )
  {
    S2_diff =  System_Base_Time - S2_ON_Time;
    if (S2_diff > S_Time)
    {
      S2_ON = 0;
      analogWrite (S2_Driver, 0);
    }
  }
  if (S3_ON == 1 )
  {
    S3_diff =  System_Base_Time -  S3_ON_Time;
    if (S3_diff > S_Time)
    {
      S3_ON = 0;
      analogWrite (S3_Driver, 0);
    }
  }
  if ( S4_ON == 1 )
  {
    S4_diff =  System_Base_Time - S4_ON_Time;
    if (S4_diff > S_Time)
    {
      S4_ON = 0;
      analogWrite (S4_Driver, 0);
    }
  }
  //
  if ( C1_ON == 1 )
  {
    C1_diff =  System_Base_Time -  C1_ON_Time;
    if (C1_diff > Chime_Time)
    {
      C1_ON = 0;
      analogWrite (C1_Driver, 0);
    }
  }
  if ( C2_ON == 1 )
  {
    C2_diff =  System_Base_Time -  C2_ON_Time;
    if (C2_diff > Chime_Time)
    {
      C2_ON = 0;
      analogWrite (C2_Driver, 0);
    }
  }
  if ( C3_ON == 1 )
  {
    C3_diff =  System_Base_Time -  C3_ON_Time;
    if (C3_diff > Chime_Time)
    {
      C3_ON = 0;
      analogWrite (C3_Driver, 0);
    }
  }
}
//
void Chime100()
{
  analogWrite(C3_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
  C3_ON = 1;
  C3_ON_Time = millis();
}
//
void Chime10()
{
  analogWrite(C2_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
  C2_ON = 1;
  C2_ON_Time = millis();
}
//
void Chime1()
{
  analogWrite(C1_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
  C1_ON = 1;
  C1_ON_Time = millis();
}
//
void loop()
{
  if (GameOver == 1)All_Coils_OFF();
  digitalWrite(LED, LOW);
  System_Base_Time = millis();
  Switch_Coils_OFF();
  // Anything from Game Contoller? (Board 1)
  if (mySerial.available() > 0)
  {
    // Read input from Controller
    ch1 = mySerial.read();
    // Replicate to TV if LCD connected rather
    // than a real backglass is being used
    Serial.print(ch1);
    digitalWrite(LED, HIGH);
    //
    if (ch1 == 'L')
    { // Set if 100 points pop bumper light
      Bumper100 = 1;
    }
    if (ch1 == 'R')
    {
      Split();
      for (int i = 0; i < 10; i++)
      {
        ResetScore();
      }
      digitalWrite (LED, HIGH);
      GameOver = 0;
      Bumper100 = 0;          // 100 points
      Serial.print ('E');     // to backglass monitor if connected
    }
    //
    if (ch1 == 'O')
    { // Game Over check for highest score
      digitalWrite(LED, LOW);
      Bumper100 = 0;
      GameOver = 1;
      if (NewScore > HighScore)
      {
        HighScore = NewScore;
        // DO SOMETHING WITH HIGHEST SCORE
      }
    }
    //
    if (ch1 == 'K')
    {
      analogWrite(KN_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
      KN_ON = 1;
      KN_ON_Time = millis();
    }
    //
    if (ch1 == 'N')
    {
      analogWrite(BD_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
      BD_ON = 1;
      BD_ON_Time = millis();
    }
    if (ch1 == 'A')
    { //  chime/Score 1 points
      AddScore = 1;
      Chime1();
    }
    //
    if (ch1 == 'B')
    { //  chime/Score 10 points
      AddScore = 10;
      Chime10();
    }
    if (ch1 == 'F')
    { //  Score 10 points No Chime
      AddScore = 10;
    }
    if (ch1 == 'C')
    { // chime/Score 100 points
      AddScore = 100;
      Chime100();
    }
  }
  if (GameOver == 0)
  {
    // Game ON
    // Check the POP Bumpers
    if ((digitalRead(B1_Switch) == 0) && ( B1_ON == 0 ))
    {
      // let switch settle
      B1_diff = System_Base_Time -  B1_ON_Time;
      if (B1_diff < Stuck_Time)GameOver = 1;
      if (B1_diff >  Wait_Time)
      {
        analogWrite(B1_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
        B1_ON = 1;
        B1_ON_Time = millis();
        if (Bumper100 > 0 )
        {
          AddScore = 100;
          Chime100();
          Serial.print ('C');
          delay(10);
        }
        else
        {
          AddScore = 10;
          Chime10();
          Serial.print ('B');
          delay(10);
        }
      }
    }
    if ((digitalRead(B2_Switch) == 0) && (  B2_ON  == 0 ))
    {
      // let switch settle
      B2_diff = System_Base_Time -  B2_ON_Time;
      if (B2_diff < Stuck_Time)GameOver = 1;
      if ( B2_diff >  Wait_Time)
      {
        analogWrite(B2_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
        B2_ON = 1;
        B2_ON_Time = millis();
        if (Bumper100 > 0 )
        {
          AddScore = 100;
          Chime100();
          Serial.print ('C');
          delay(10);
        }
        else
        {
          AddScore = 10;
          Chime10();
          Serial.print ('B');
          delay(10);
        }
      }
    }
    if ((digitalRead(B3_Switch) == 0) && ( B3_ON == 0 ))
    {
      // let switch settle
      B3_diff = System_Base_Time -  B3_ON_Time;
      if (B3_diff < Stuck_Time)GameOver = 1;
      if ( B3_diff > Wait_Time)
      {
        analogWrite(B3_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
        B3_ON = 1;
        B3_ON_Time = millis();
        AddScore = 100;
        Chime100();
        Serial.print ('C');
        delay(10);
      }
    }
    //
    if (AddScore > 0)
    {
      if (AddScore == 100)
      {
        analogWrite(S3_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
        S3_ON = 1;
        S3_ON_Time = millis();
      }
      if (AddScore == 10)
      {
        analogWrite(S2_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
        S2_ON = 1;
        S2_ON_Time = millis();
      }
      if (AddScore == 1)
      {
        analogWrite(S1_Driver,  Coil_PWM_100 );  //Coil Solenoid activated !
        S1_ON = 1;
        S1_ON_Time = millis();
      }
      NewScore = NewScore + AddScore;
      if (NewScore > 9999)NewScore = 9999;
      //
      AddScore = 0;
      Split();
      if (S4 > Old_S4 )
      {
        Old_S4 = S4;
        analogWrite(S4_Driver, Coil_PWM_100 );  //Coil Solenoid activated !
        S4_ON = 1;
        S4_ON_Time = millis();
      }
    }
  }
}
