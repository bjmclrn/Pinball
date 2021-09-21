/*
  By B J Mclaren 8/09/2021
  Used for Pinball flipper control.
  Controller is the COBRAPIN Expansion Board with 8 Drivers.
  NOTE because PB10 and PB11 are not available only via the
  STM32DUINO IDE only 6 drivers are available.

  Lower playfield flippers are twin coil and require
  2 drivers per flipper. Power is supplied to the tab on the coil
  where 2 wires connected together. The COBRAPIN MOSFET's provide
  the return path when activated.

  If upper playfield flippers are present
  then they will have to be PWM flippers using a single coil.

  Flippers are returns are enable from Relays controlled by the
  COBRAPIN Controller Board 1 on pin 1-0-1

  Feel free to use and update to meet needs.
*/
// Playfield Flipper Switches J2
#define LFlipper_Switch        PA7   // 2-0-27
#define LFlipper_Return        PA6   // 2-0-26
#define RFlipper_Switch        PA5   // 2-0-25
#define RFlipper_Return        PA4   // 2-0-24
//
// Playfield Flipper Drivers J6
#define Hold_RFlipper_Driver   PA0   // 2-0-8
#define Power_RFlipper_Driver  PA1   // 2-0-9
#define Hold_LFlipper_Driver   PA2   // 2-0-10
#define Power_LFlipper_Driver  PA3   // 2-0-11
//
#define LED PB2
//
// If using a 25 volt PSU or greater and coils below 2.5 Ohms
// Flippers power settings are: weak=160; medium=190; strong>210;
int Power_PWM = 195;
// Flipper Hold setting depending on PSU/Coil combination for
// 40 ohms hold coil and 25 volts; use maximum setting
int Hold_PWM = 255;                 // Max setting
int PWM_OFF  = 0;
//
unsigned long System_Time = 0;
unsigned long LFlipper_ON_Time = 0;
unsigned long RFlipper_ON_Time = 0;
// Allow enough time for the Power Flipper to fully deploy
// usually 60 millis is OK
unsigned long Flipper_ON_Time = 60;
unsigned long diff = 0;
// Flags to indicate Flipper Condition
int LFlipper_ON = 0;
int RFlipper_ON = 0;
int LF_Power_OFF = 0;
int RF_Power_OFF = 0;

//
void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  //
  pinMode(LFlipper_Switch,  INPUT_PULLUP);
  pinMode(RFlipper_Switch,  INPUT_PULLUP);
  pinMode(LFlipper_Return,  OUTPUT);
  pinMode(RFlipper_Return,  OUTPUT);
  digitalWrite(LFlipper_Return, LOW);
  digitalWrite(RFlipper_Return, LOW);
  //
  pinMode (Power_LFlipper_Driver, OUTPUT);
  pinMode (Power_RFlipper_Driver, OUTPUT);
  analogWrite(Power_LFlipper_Driver, PWM_OFF);
  analogWrite(Power_RFlipper_Driver, PWM_OFF);
  pinMode (Hold_LFlipper_Driver, OUTPUT);
  pinMode (Hold_RFlipper_Driver, OUTPUT);
  analogWrite(Hold_LFlipper_Driver, PWM_OFF);
  analogWrite(Hold_RFlipper_Driver, PWM_OFF);
  //
}
//
void Switch_Flippers_OFF()
{
  if ((digitalRead(LFlipper_Switch) == HIGH) && (LFlipper_ON == HIGH))
  {
    LFlipper_ON = LOW;
    LF_Power_OFF = LOW;
    analogWrite(Power_LFlipper_Driver, PWM_OFF);
    analogWrite(Hold_LFlipper_Driver,PWM_OFF);
  }
  //
  if ((digitalRead(RFlipper_Switch) == HIGH) && (RFlipper_ON == HIGH))
  {
    RFlipper_ON = LOW;
    RF_Power_OFF = LOW;
    analogWrite(Power_RFlipper_Driver, PWM_OFF);
    analogWrite(Hold_RFlipper_Driver, PWM_OFF);
  }
}
//
void loop()
{
  System_Time = millis();
  Switch_Flippers_OFF();
  //
  if (RFlipper_ON == HIGH )
  {
    diff =  System_Time - RFlipper_ON_Time;
    if ((diff > Flipper_ON_Time) && ( RF_Power_OFF == LOW ))
    {
      // Power Driver OFF
      analogWrite(Power_RFlipper_Driver, PWM_OFF);
      RF_Power_OFF = HIGH;
    }
  }
  if (LFlipper_ON == HIGH )
  {
    diff =  System_Time - LFlipper_ON_Time;
    if ((diff > Flipper_ON_Time) && ( LF_Power_OFF == LOW ))
    {
      // Power Driver OFF
      analogWrite(Power_LFlipper_Driver, PWM_OFF);
      LF_Power_OFF = HIGH;
    }
  }
  if ((digitalRead(LFlipper_Switch) == LOW) && (LFlipper_ON == LOW))
  {
    // Both Coils ON
    LFlipper_ON_Time = millis();
    analogWrite(Power_LFlipper_Driver, Power_PWM);
    analogWrite(Hold_LFlipper_Driver,  Hold_PWM);
    LFlipper_ON = HIGH;
  }
  if ((digitalRead(RFlipper_Switch) == LOW) && (RFlipper_ON == LOW))
  {
    // Both Coils ON
    RFlipper_ON_Time = millis();
    analogWrite(Power_RFlipper_Driver, Power_PWM);
    analogWrite(Hold_RFlipper_Driver,  Hold_PWM);
    RFlipper_ON = HIGH;
  }
  //
}
