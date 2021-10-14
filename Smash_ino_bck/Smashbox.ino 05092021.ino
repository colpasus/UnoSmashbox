#include "Smashbox.h"
boolean led = false;
int pinled1 = 13;
int pinled2 = A5;
int pinled3 = A3;
int pinled4 = A4;
int pinb = 12;


float CountDownNoPushEfect=0;
float CountDownNoPushEfectConst=5000;  // 5 secs no push activate effect.



//WORKING ON IT. NOT TESTED YET

boolean TiltSpecialButtonZL_ON=false;  // We can use ZL to TILT BOTTON IF PRESS MINUS/PLUS/ZL AT THE SAME TIME


#define KICKOFFCHEATS

#ifdef KICKOFFCHEATS

boolean KOOL_STOPANDRUN=false;  // We can use pin_Y for a fast stop and go
boolean KOOL_AUTOLOB=false;  // We can use pin_A for a LOB

#endif




int TiltDoubleClick=0;   // If push right and left direction stick at the same time, tilt to the direction of the first pushed (walking, not running).

const int FactorTilt=64;

// -1 LEFT
// 0 NO
// 1 RIGHT

int pin_X=2;
int pin_Y=3;
int pin_A=4;
int pin_B=5;

int pin_dpadUpOn = 6;
int pin_dpadDownOn=7;
int pin_dpadLeftOn=8;
int pin_dpadRightOn=9;

int LeftStick_Top_pin   =  30  ;
int LeftStick_Right_pin =  31  ;
int LeftStick_Down_pin  =  32  ;
int LeftStick_Left_pin  =  33  ;


int RightStick_Top_pin   =  34 ;
int RightStick_Right_pin =  35 ;
int RightStick_Down_pin  =  36 ;
int RightStick_Left_pin  =  37 ;


int pin_SWITCH_L   =  22 ;
int pin_SWITCH_R =    23 ;
int pin_SWITCH_ZL  =  24 ;
int pin_SWITCH_ZR  =  25 ;

int pin_LEFT_3 = 38;
int pin_RIGHT_3 = 39;


int pin_MinusOn= 26;
int pin_PlusOn=27;
int pin_HomeOn=28;



// Smashbox needs a way to avoid running in some situations.
// You can use ZL or a special button out of the set of Nintendo Switch buttons.
// THe special button will be attached to the pin 29
int pin_TILT=29;



int myLedPins[] = {40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
int PointerLed=0;


// LEDS

// PACK 1. 40 to 53 (15 leds)

int Led_pin_X=40;
int Led_pin_Y=41;
int Led_pin_A=42;
int Led_pin_B=43;

int Led_pin_SWITCH_L   =  44 ;
int Led_pin_SWITCH_R =    45 ;
int Led_pin_SWITCH_ZL  =  46 ;
int Led_pin_SWITCH_ZR  =  47 ; 

int Led_LeftStick_Top_pin   =  48  ;
int Led_LeftStick_Right_pin =  49  ;
int Led_LeftStick_Down_pin  =  50  ;
int Led_LeftStick_Left_pin  =  51  ;



float oldTime,currentTime,oldLedsTime,currentTimeLedsRotate;
float InitialTime;
float InitialLedsSequence=5000; //10 secs

float RotateSpeed=1000; //10 secs
float RotateSpeedInitial=1000; //10 secs





void setup(){

  setupPins();
  setupUnoJoy();

 
  pinMode(pinled1, OUTPUT);
  pinMode(pinled2, OUTPUT);
  pinMode(pinled3, OUTPUT);
  pinMode(pinled4, OUTPUT);
  pinMode(pinb, INPUT_PULLUP);

  InitialTime=millis();
 
}




void loop(){
  if (digitalRead(pinb) == LOW){
    digitalWrite(pinled1, HIGH);
    digitalWrite(pinled2, HIGH);
    digitalWrite(pinled3, HIGH);
    digitalWrite(pinled4, HIGH);
  }
  else if (digitalRead(pinb) == HIGH){
    if (digitalRead(A1) == LOW){
      led = true;
      }
    else if (digitalRead(A0) == LOW){
      led = true;
      }
    else {
      led = false;
      }
    if (led == true){
      digitalWrite(pinled1, HIGH);
      }
    else {
      digitalWrite(pinled1, LOW);
      }
    if (digitalRead(4) == LOW){
      led = true;
      }
    else if (digitalRead(5) == LOW){
      led = true;
      }
    else {
      led = false;
      }
    if (led == true){
      digitalWrite(pinled2, HIGH);
      }
    else {
      digitalWrite(pinled2, LOW);
      }
    if (digitalRead(3) == LOW){
      led = true;
      }
    else if (digitalRead(2) == LOW){
      led = true;
      }
    else {
      led = false;
      }
    if (led == true){
      digitalWrite(pinled3, HIGH);
      }
    else {
      digitalWrite(pinled3, LOW);
      }
    if (digitalRead(11) == LOW){
      led = true;
      }
    else if (digitalRead(10) == LOW){
      led = true;
      }
    else {
      led = false;
      }
    if (led == true){
      digitalWrite(pinled4, HIGH);
      }
    else {
      digitalWrite(pinled4, LOW);
      }
  }


  
  currentTime=millis();
  if (  (currentTime-InitialTime) > InitialLedsSequence)
  {
    dataForController_t controllerData = getControllerData();
    setControllerData(controllerData);
  }
  else
     StartLedsSequence();
  

}


void setupPins(void){
  for (int i = 2; i <= 11; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }


//MEGA SET OF BUTTONS

  for (int i = 22; i <= 39; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }

  
  pinMode(A1, INPUT);
  digitalWrite(A1, HIGH);
  pinMode(A0, INPUT);
  digitalWrite(A0, HIGH);


//MEGA LEDS

  for (int i = 40; i <= 53; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }


}




dataForController_t getControllerData(void){
  dataForController_t controllerData = getBlankDataForController();






// SET XYAB

  controllerData.triangleOn = !digitalRead(pin_X);
  controllerData.circleOn = !digitalRead(pin_Y);
  controllerData.squareOn = !digitalRead(pin_A);
  controllerData.crossOn = !digitalRead(pin_B);


// SET DPAD  
  controllerData.dpadUpOn = !digitalRead(pin_dpadUpOn);
  controllerData.dpadDownOn = !digitalRead(pin_dpadDownOn);
  controllerData.dpadLeftOn = !digitalRead(pin_dpadLeftOn);
  controllerData.dpadRightOn = !digitalRead(pin_dpadRightOn);


 /*  
  controllerData.l1On = !digitalRead(10);
  controllerData.r1On = !digitalRead(11);
  controllerData.MinusOn = !digitalRead(A0);
  controllerData.PlusOn = !digitalRead(A1);
*/

// SET ANALOGIC LEFT (NO RIGHT)




// TILT BUTTON LOGIC

// IF TILT BUTTON PUSHED OR ZL (IF ENABLED) LEFT STICK WALK
// IF NOT TILT AND NOT ZL (IF ENABLED) LEFT STICK RUN
// THIS IS FOR SMASH BROS GAME OR SIMILAR






// TILT CAN BE IF DOUBLE DIRECTION AT THE SAME TIME OR 1 DIRECTION PLUS TILT BUTTON / ZL



if ( !digitalRead (LeftStick_Right_pin) && !digitalRead (LeftStick_Left_pin) )   //  DOUBLE DIRECTION, LETS SEE WHAT DIRECTION WAS FIRST
{

if ( digitalRead ( pin_TILT) && digitalRead(pin_SWITCH_ZL))   // I CAN RUN, WALK OR GO STEP BY STEP. 
  { // THIS IS THE CASE FOR WALK
    if (TiltDoubleClick==-1) controllerData.leftStickX=   0  + FactorTilt;   // STICK TO MEDIUM
    if (TiltDoubleClick==1) controllerData.leftStickX=   255 - FactorTilt;   // STICK TO MEDIUM
  }  
  else
  {   // AND THIS IS THE CASE OF STEP BY STEP (DOUBLE DIRECTION + BUTTON ZL OR TILT) 
    if (TiltDoubleClick==-1) controllerData.leftStickX=   128 - FactorTilt/2;
    if (TiltDoubleClick==1) controllerData.leftStickX=   128 + FactorTilt/2;
  }
}
else
{
  if ( digitalRead ( pin_TILT) && digitalRead(pin_SWITCH_ZL))   // NO DOUBLE DIRECTION, LETS SEE IF ZL OR TILT BUTTON IS PUSHED
  {  //RUN
    if (!digitalRead (LeftStick_Right_pin))     controllerData.leftStickX=255;
    if (!digitalRead (LeftStick_Left_pin))     controllerData.leftStickX=0;
  }
  else
  {  // WALK DIRECTION LEFT OR RIGHT
    if (!digitalRead (LeftStick_Right_pin))     controllerData.leftStickX= 255 - FactorTilt;
    if (!digitalRead (LeftStick_Left_pin))     controllerData.leftStickX=    0 + FactorTilt;
  }
}


// UP AND DOWN

if ( digitalRead ( pin_TILT) && digitalRead(pin_SWITCH_ZL))   // ZL because I dont't have enough buttons
{
  if (!digitalRead (LeftStick_Top_pin))     controllerData.leftStickY=0;
  if (!digitalRead (LeftStick_Down_pin))     controllerData.leftStickY=255;
}
else
{
  if (!digitalRead (LeftStick_Top_pin))     controllerData.leftStickY=64;
  if (!digitalRead (LeftStick_Down_pin))     controllerData.leftStickY=128+64;
}


/////////////////////////////////////////////////////////////



  if (digitalRead(pin_TILT) == LOW){
    digitalWrite(pinled1, HIGH);
    digitalWrite(pinled2, HIGH);
    digitalWrite(pinled3, HIGH);
    digitalWrite(pinled4, HIGH);
  }


// SET L ZL L3 R ZR R3

  controllerData.l1On = !digitalRead(pin_SWITCH_L);
  if (!TiltSpecialButtonZL_ON)  controllerData.l2On = !digitalRead(pin_SWITCH_ZL);   // IF  TiltSpecialButtonZL_ON=FALSE THEN ZL WORKS.
  controllerData.l3On = !digitalRead(pin_LEFT_3);

  controllerData.r1On = !digitalRead(pin_SWITCH_R);
  controllerData.r2On = !digitalRead(pin_SWITCH_ZR);
  controllerData.r3On = !digitalRead(pin_RIGHT_3);


// HOME SELECT START


  controllerData.MinusOn = !digitalRead(pin_MinusOn);
  controllerData.PlusOn = !digitalRead(pin_PlusOn);
  controllerData.HomeOn = !digitalRead(pin_HomeOn);



  if ( !digitalRead (LeftStick_Left_pin) && digitalRead  (LeftStick_Right_pin) ) TiltDoubleClick=-1; // Actually running LEFT
  if ( digitalRead  (LeftStick_Left_pin) && !digitalRead (LeftStick_Right_pin) ) TiltDoubleClick=1; // Actually running LEFT
  if ( digitalRead  (LeftStick_Left_pin) && digitalRead  (LeftStick_Right_pin) ) TiltDoubleClick=0; // NO RIGHT AND NO LEFT

  if ( !digitalRead (LeftStick_Left_pin) && digitalRead  (!LeftStick_Right_pin) ) TiltDoubleClick=TiltDoubleClick; // I WAS TILTING AND KEEP TILTING THE SAME DIRECTION, NO CHANGES

  
// ACTIVATE AND DESACTIVATE TILT BUTTON 
// ONLY USE IT IF YOU HAVE NOT A SPECIAL TILT BUTTON, YOU CAN USE ZL TO DO IT, BUT THE ZL IS DISABLED 
// TO ACTIVATE, PRESS (A+B+X+Y+ZL) AND THEN RELEASE (A+B+X+Y) KEEPING ZL


/*
   if ( !digitalRead (pin_MinusOn) && digitalRead  (!pin_PlusOn) && digitalRead(!pin_HomeOn)  &&  !digitalRead(pin_SWITCH_ZL)  ) TiltSpecialButtonZL_ON=true;
   if ( !digitalRead (pin_MinusOn) && digitalRead  (!pin_PlusOn) && digitalRead(!pin_HomeOn)  &&  digitalRead(pin_SWITCH_ZL)  ) TiltSpecialButtonZL_ON=false;
*/


   if ( !digitalRead (pin_X) && digitalRead  (!pin_Y) && digitalRead(!pin_A)  &&  digitalRead(!pin_B)  && !digitalRead(pin_SWITCH_ZL)  ) TiltSpecialButtonZL_ON=true;
   if ( !digitalRead (pin_X) && digitalRead  (!pin_Y) && digitalRead(!pin_A)  &&  digitalRead(!pin_B)  && digitalRead(pin_SWITCH_ZL)  ) TiltSpecialButtonZL_ON=false;


// KICK OFF CHEAT. LOB AND STOP AND RUN

//compile this code if macro_name exists
#ifdef KICKOFFCHEATS

   if ( !digitalRead (LeftStick_Top_pin) && digitalRead  (!LeftStick_Right_pin) && digitalRead(!LeftStick_Down_pin)  &&  !digitalRead(LeftStick_Left_pin)  && !digitalRead(pin_Y)  ) KOOL_STOPANDRUN=true; 
   if ( !digitalRead (LeftStick_Top_pin) && digitalRead  (!LeftStick_Right_pin) && digitalRead(!LeftStick_Down_pin)  &&  !digitalRead(LeftStick_Left_pin)  && digitalRead(pin_Y)   ) KOOL_STOPANDRUN=false;

   if ( !digitalRead (LeftStick_Top_pin) && digitalRead  (!LeftStick_Right_pin) && digitalRead(!LeftStick_Down_pin)  &&  !digitalRead(LeftStick_Left_pin)  && !digitalRead(pin_A)  ) KOOL_AUTOLOB=true; 
   if ( !digitalRead (LeftStick_Top_pin) && digitalRead  (!LeftStick_Right_pin) && digitalRead(!LeftStick_Down_pin)  &&  !digitalRead(LeftStick_Left_pin)  && digitalRead(pin_A)   ) KOOL_AUTOLOB=false;
   
#endif




//LEDS STYLE NORMAL. IF PUSH, TURN ON


  for (int i=pin_X; i<=pin_B ; ++i)
    if (!digitalRead(i)) 
      digitalWrite( i + (Led_pin_X-pin_X) , HIGH);
    else
      digitalWrite( i + (Led_pin_X-pin_X) , LOW);


  for (int i=pin_SWITCH_L; i<=pin_SWITCH_ZR ; ++i)
    if (!digitalRead(i)) 
      digitalWrite( i + (Led_pin_SWITCH_L-pin_SWITCH_L) , HIGH);
    else
      digitalWrite( i + (Led_pin_SWITCH_L-pin_SWITCH_L) , LOW);


  for (int i=pin_SWITCH_L; i<=pin_SWITCH_ZR ; ++i)
    if (!digitalRead(i)) 
      digitalWrite( i + (Led_pin_SWITCH_L-pin_SWITCH_L) , HIGH);
    else
      digitalWrite( i + (Led_pin_SWITCH_L-pin_SWITCH_L) , LOW);


  for (int i=LeftStick_Top_pin; i<=LeftStick_Left_pin ; ++i)
    if (!digitalRead(i)) 
      digitalWrite( i + (Led_LeftStick_Top_pin-LeftStick_Top_pin) , HIGH);
    else
      digitalWrite( i + (Led_LeftStick_Top_pin-LeftStick_Top_pin) , LOW);





// Light effect if no button pressed.


  ControlLightEffect();


 
  return controllerData;
}


void StartLedsSequence(void)
{
  TurnOnAllLeds();
}

void TurnOffAllLeds(void)
{
  for (int i=Led_pin_X; i<=Led_LeftStick_Left_pin; ++i)
  {
    digitalWrite(i,LOW);
  }
}


void TurnOnAllLeds(void)
{
  for (int i=Led_pin_X; i<=Led_LeftStick_Left_pin; ++i)
  {
    digitalWrite(i,HIGH);
  }
}

void ControlLightEffect()
{
  boolean buttonPushed=false;

  for (int i=pin_X; i<=pin_B ; ++i)
    if (!digitalRead(i)) 
      buttonPushed=true;


  for (int i=pin_SWITCH_L; i<=pin_SWITCH_ZR ; ++i)
    if (!digitalRead(i)) 
      buttonPushed=true;


  for (int i=pin_SWITCH_L; i<=pin_SWITCH_ZR ; ++i)
    if (!digitalRead(i)) 
      buttonPushed=true;


  for (int i=LeftStick_Top_pin; i<=LeftStick_Left_pin ; ++i)
    if (!digitalRead(i)) 
      buttonPushed=true;


  currentTime=millis();

  if (buttonPushed) 
  {
    CountDownNoPushEfect=0;  //reseting time.
    RotateSpeed=RotateSpeedInitial; 
  }
  else
  CountDownNoPushEfect += (currentTime-oldTime);  // +Delta

  if (CountDownNoPushEfect>CountDownNoPushEfectConst)
    RunEffect();

  
  oldTime=currentTime;
   
  
}


void RunEffect()
{
    currentTime=millis();

    currentTimeLedsRotate += (currentTime-oldLedsTime);
    if (  currentTimeLedsRotate > RotateSpeed )
    {

      currentTimeLedsRotate=0;  // Reset. Wait for next iteration.
      
      TurnOnAllLeds();

      ++PointerLed;
      if (PointerLed> 11) PointerLed=0;      

      digitalWrite(PointerLed,HIGH);
    }

    oldLedsTime=currentTime;
}
