// J3 Switches
#define Cswitch      PB5        // 1-0-11      
#define Aswitch      PB4        // 1-0-10          
#define Rswitch      PB3        // 1-0-9         
#define Pswitch      PA15       // 1-0-8          
#define Eswitch      PB14       // 1-0-3          
#define Tswitch      PB13       // 1-0-2          
//   
// J4 Switches
#define Fswitch      PA4        // 1-0-24
#define Lswitch      PA5        // 1-0-25
#define Yswitch      PA6        // 1-0-26
#define Iswitch      PA7        // 1-0-27
#define Nswitch      PB0        // 1-0-28
#define Gswitch      PB1        // 1-0-29
//
// J5 Switches
#define Startbutton  PA3        // 1-0-23
#define Drain_Switch PA2        // 1-0-22
#define DATA_PIN     PA1        // 1-0-21
#define Slingshot    PA0        // 1-0-20  
#define Tilt         PC13       // 1-0-17 
// J8 Drivers
#define BlueLed_Driver PA10     // Lit when 100 points
// 1-0-1 Reserved for relay
#define Flipper_Control PA8     // 1-0-1
//#define LED PC13              // On Board LED OLD Style Jumpers
#define LED PB2                 // On Board LED NEW Style Buttons
#define NUM_LEDS 70             //Space for up to 70 LEDS 
#define No_of_Switches 15       // 14 switches + spare
//
int Coil_On_Time = 50;          // Coil on time in ms
int Coil_PWM_100 = 255;         // 100% power
int Coil_PWM_90  = 230;         // approx 90%
int Coil_PWM_75  = 190;         // approx 75%
//
