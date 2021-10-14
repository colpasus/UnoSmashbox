#include "Smashbox.h"
boolean led = false;
int pinled1 = 13;
int pinled2 = A5;
int pinled3 = A3;
int pinled4 = A4;
int pinb = 12;


boolean TiltSpecialButtonZL_ON=false;  // We can use ZL to TILT BOTTON IF PRESS MINUS/PLUS/HOME AT THE SAME TIME

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







void setup(){

  setupPins();
  setupUnoJoy();

 
  pinMode(pinled1, OUTPUT);
  pinMode(pinled2, OUTPUT);
  pinMode(pinled3, OUTPUT);
  pinMode(pinled4, OUTPUT);
  pinMode(pinb, INPUT_PULLUP);
 
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

  
  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);


}


void setupPins(void){
  for (int i = 2; i <= 11; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }


//MEGA SET OF BUTTONS

  for (int i = 22; i <= 49; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }

  
  pinMode(A1, INPUT);
  digitalWrite(A1, HIGH);
  pinMode(A0, INPUT);
  digitalWrite(A0, HIGH);
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
  if (!TiltSpecialButtonZL_ON)  controllerData.l2On = !digitalRead(pin_SWITCH_ZL);   // IF NOT TiltSpecialButtonZL_ON THEN ZL WORKS.
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


   if ( !digitalRead (pin_MinusOn) && digitalRead  (!pin_PlusOn) && digitalRead(!pin_HomeOn)  &&  !digitalRead(pin_SWITCH_ZL)  ) TiltSpecialButtonZL_ON=true;

   if ( !digitalRead (pin_MinusOn) && digitalRead  (!pin_PlusOn) && digitalRead(!pin_HomeOn)  &&  digitalRead(pin_SWITCH_ZL)  ) TiltSpecialButtonZL_ON=false;


 
  return controllerData;
}
