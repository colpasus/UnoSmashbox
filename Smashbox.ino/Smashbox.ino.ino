#include "Smashbox.h"
boolean led = false;
int pinled1 = 13;
int pinled2 = A5;
int pinled3 = A3;
int pinled4 = A4;
int pinb = 12;

//Version 1.1 Combo record and play added
// colpasus 12/09/2021

float CountDownNoPushEffect=0;
float CountDownNoPushEffectConst=5000;  // 5 secs no push activate effect.



//WORKING ON IT. NOT TESTED YET

boolean TiltSpecialButtonZL_ON=false;  // We can use ZL to TILT BOTTON IF PRESS MINUS/PLUS/ZL AT THE SAME TIME


//#define DEBUG

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





int PointerLed=0;
boolean RunEffectOn;
boolean LightOnOff=true;


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



float oldTime,currentTime,oldLedsTime,currentTimeLedsRotate,startTimeComboRecord,startTimeComboPlay,BlinkTime=0,LastTime=0;
float InitialTime;
float InitialLedsSequence=2500; //10 secs

float RotateSpeed=800; //10 secs
float RotateSpeedInitial=500; //10 secs



int myLedPins[] = {
 Led_pin_SWITCH_R,
 Led_pin_SWITCH_L,
 Led_pin_X,
 Led_pin_A,
 Led_LeftStick_Top_pin,
 Led_LeftStick_Left_pin,
 Led_LeftStick_Down_pin,
 Led_LeftStick_Right_pin,
 Led_pin_B,
 Led_pin_Y,
 Led_pin_SWITCH_ZL,
 Led_pin_SWITCH_ZR 
 };


/////////////////////////////////////////////////////////////////////
// Recording control
/////////////////////////////////////////////////////////////////////

boolean PreparingForRecording=false;
boolean Recording=false;
boolean Playing=false;
const int MaxRecordingSteps=500;
int RecordingStep=0;
int PlayingStep=0;

float RecordingTime=0;
dataForController_t LastRecordingData;

//  const byte *PointerToDataNew = (const byte*) &controllerData;

//  const byte *PointerToDataOld = (const byte*) &LastRecordingData;


typedef struct
  {
      dataForController_t data;
      float Timestamp;
  }  record_type;

record_type RecordingSteps[MaxRecordingSteps];


  const byte *PointerToDataHist = (const byte*) &RecordingSteps[0].data;


/////////////////////////////////////////////////////////////////////
// End recording control
/////////////////////////////////////////////////////////////////////


void setup(){

#ifdef DEBUG  
  Serial.begin(9600);
  Serial.println("Starting Program");
#endif

  setupPins();

#ifndef DEBUG  
  setupUnoJoy();  //// RESTORE
#endif
 
  pinMode(pinled1, OUTPUT);
  pinMode(pinled2, OUTPUT);
  pinMode(pinled3, OUTPUT);
  pinMode(pinled4, OUTPUT);
  pinMode(pinb, INPUT_PULLUP);

  InitialTime=millis();
 
}




void loop(){


  currentTime=millis();
  if (  (currentTime-InitialTime) > InitialLedsSequence)
  {
    dataForController_t controllerData = getControllerData();


#ifndef DEBUG  
  setControllerData(controllerData);
#endif
    


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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// MAIN PROCESS /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////



dataForController_t getControllerData(void){
  
  dataForController_t controllerData = getBlankDataForController();

  byte *PointerToDataNew = ( byte*) &controllerData;
  byte *PointerToDataOld = ( byte*) &LastRecordingData;


  if (AnyButtonPressed() && Playing==true)  // If combo running, if any button is pressed the combo is finished.
  {

#ifdef DEBUG  
  Serial.println("Reset LOOP");
#endif

    
        PreparingForRecording=false;
        Recording=false;
        Playing=false;
        PlayingStep=0;      
  }


  
  
// Recording control of buttons (SPECIAL + PLUS)
// If both buttons are pressed, no send nothing and preparing for start to record.
if ( !digitalRead ( pin_TILT) && !digitalRead(pin_PlusOn) && !Recording)
{

#ifdef DEBUG  
  Serial.println("Preparing for recording");
#endif


  
  Recording=false;
  Playing=false;
  RecordingStep=0;
  RecordingTime=0;
  PlayingStep=0;
  
  PreparingForRecording=true;

  PointerToDataOld=PointerToDataNew;

// If recording combo NOT send special buttons.
  controllerData.MinusOn = 0;
  controllerData.PlusOn = 0;
  controllerData.HomeOn = 0;

  return controllerData;
}


// Recording control of buttons (SPECIAL + MINUS)
// If both buttons are pressed, no send nothing and stop recording.
if ( !digitalRead ( pin_TILT) && !digitalRead(pin_MinusOn) && Recording==true)
{

  PreparingForRecording=false;
  Recording=false;
  Playing=false;
  PlayingStep=0;
 

// If recording combo NOT send special buttons.
  controllerData.MinusOn = 0;
  controllerData.PlusOn = 0;
  controllerData.HomeOn = 0;
  
  return controllerData;
}


//If push TILG + HomeOn recording is stoped, we can play
if ( !digitalRead ( pin_TILT) && !digitalRead(pin_HomeOn) && Recording==false && Playing==false)
{

  PreparingForRecording=false;
  Recording=false;
  Playing=true;

#ifdef DEBUG  
    Serial.println("PLAY PRESSED");
#endif

// If recording combo NOT send special buttons.
  controllerData.MinusOn = 0;
  controllerData.PlusOn = 0;
  controllerData.HomeOn = 0;


  PlayingStep=0;


}

if ( Playing )  // IF HERE, PLAYING THE COMBO
{
  


  if ( PlayingStep == 0 )  //First time only
     startTimeComboPlay=millis();

  currentTime=millis();
  
  float deltaTimeCombo=currentTime-startTimeComboPlay;


  if ( deltaTimeCombo >= RecordingSteps[PlayingStep].Timestamp ) //oscar
  { //New sequence of buttons
    //Execute buttons

 
                    controllerData.triangleOn = RecordingSteps[PlayingStep].data.triangleOn;
                    controllerData.circleOn = RecordingSteps[PlayingStep].data.circleOn;
                    controllerData.squareOn = RecordingSteps[PlayingStep].data.squareOn;
                    controllerData.crossOn = RecordingSteps[PlayingStep].data.crossOn;
                    controllerData.l1On = RecordingSteps[PlayingStep].data.l1On;
                    controllerData.l2On = RecordingSteps[PlayingStep].data.l2On;
                    controllerData.l3On = RecordingSteps[PlayingStep].data.l3On;
                    controllerData.r1On = RecordingSteps[PlayingStep].data.r1On;
                    controllerData.r2On = RecordingSteps[PlayingStep].data.r2On;
                    controllerData.r3On = RecordingSteps[PlayingStep].data.r3On;
                    controllerData.dpadLeftOn = RecordingSteps[PlayingStep].data.dpadLeftOn;
                    controllerData.dpadUpOn = RecordingSteps[PlayingStep].data.dpadUpOn;
                    controllerData.dpadRightOn = RecordingSteps[PlayingStep].data.dpadRightOn;
                    controllerData.dpadDownOn = RecordingSteps[PlayingStep].data.dpadDownOn;  
                    controllerData.MinusOn = RecordingSteps[PlayingStep].data.MinusOn;
                    controllerData.PlusOn = RecordingSteps[PlayingStep].data.PlusOn;
                    controllerData.HomeOn = RecordingSteps[PlayingStep].data.HomeOn;

                    controllerData.leftStickX = RecordingSteps[PlayingStep].data.leftStickX;
                    controllerData.leftStickY = RecordingSteps[PlayingStep].data.leftStickY;
                    controllerData.rightStickX = RecordingSteps[PlayingStep].data.rightStickX;
                    controllerData.rightStickY = RecordingSteps[PlayingStep].data.rightStickY;


//                    if ( controllerData.triangleOn )  digitalWrite(Led_pin_X,LOW);      else                   digitalWrite(Led_pin_X,HIGH);  

                    WriteLedsFromPlaying(controllerData);



                     CountDownNoPushEffect=0;  //reseting time.
                     RotateSpeed=RotateSpeedInitial; 
                     RunEffectOn=false;

//                     NormalLedEffect(); 
/*
    const byte *MyPointerToDataArray = (byte*) &RecordingSteps[PlayingStep].data;                
    PointerToDataNew=MyPointerToDataArray;
*/

#ifdef DEBUG  
        Serial.println();
        Serial.print("[");
        Serial.print(controllerData.triangleOn,BIN);
        Serial.print(controllerData.circleOn,BIN);
        Serial.print(controllerData.squareOn,BIN);
        Serial.print(controllerData.crossOn,BIN);
        Serial.print("]  ");
        Serial.print(PlayingStep);
        Serial.print(".....");
        Serial.print(RecordingSteps[PlayingStep].Timestamp);
        Serial.print(" / ");
        Serial.print(deltaTimeCombo);
        
#endif





                    
       PlayingStep++;
       if (PlayingStep>RecordingStep)   
       {
    
           #ifdef DEBUG  
              Serial.println("");
              Serial.println("SEQUENCE FINISHED, NEXT LOOP --------------------");
           #endif

/*        
        PreparingForRecording=false;
        Recording=false;
        Playing=false;
        PlayingStep=0;      
 */
        PlayingStep=0;  // Restart loop.
    

    }

  }
  else
  {  //Old Sequence of buttons

                    controllerData.triangleOn = RecordingSteps[PlayingStep-1].data.triangleOn;
                    controllerData.circleOn = RecordingSteps[PlayingStep-1].data.circleOn;
                    controllerData.squareOn = RecordingSteps[PlayingStep-1].data.squareOn;
                    controllerData.crossOn = RecordingSteps[PlayingStep-1].data.crossOn;
                    controllerData.l1On = RecordingSteps[PlayingStep-1].data.l1On;
                    controllerData.l2On = RecordingSteps[PlayingStep-1].data.l2On;
                    controllerData.l3On = RecordingSteps[PlayingStep-1].data.l3On;
                    controllerData.r1On = RecordingSteps[PlayingStep-1].data.r1On;
                    controllerData.r2On = RecordingSteps[PlayingStep-1].data.r2On;
                    controllerData.r3On = RecordingSteps[PlayingStep-1].data.r3On;
                    controllerData.dpadLeftOn = RecordingSteps[PlayingStep-1].data.dpadLeftOn;
                    controllerData.dpadUpOn = RecordingSteps[PlayingStep-1].data.dpadUpOn;
                    controllerData.dpadRightOn = RecordingSteps[PlayingStep-1].data.dpadRightOn;
                    controllerData.dpadDownOn = RecordingSteps[PlayingStep-1].data.dpadDownOn;  
                    controllerData.MinusOn = RecordingSteps[PlayingStep-1].data.MinusOn;
                    controllerData.PlusOn = RecordingSteps[PlayingStep-1].data.PlusOn;
                    controllerData.HomeOn = RecordingSteps[PlayingStep-1].data.HomeOn;

                    controllerData.leftStickX = RecordingSteps[PlayingStep-1].data.leftStickX;
                    controllerData.leftStickY = RecordingSteps[PlayingStep-1].data.leftStickY;
                    controllerData.rightStickX = RecordingSteps[PlayingStep-1].data.rightStickX;
                    controllerData.rightStickY = RecordingSteps[PlayingStep-1].data.rightStickY;


                    return controllerData;

/*
    const byte *MyPointerToDataArray_1 = (byte*) &RecordingSteps[PlayingStep-1].data;                
    PointerToDataNew=MyPointerToDataArray_1;    
    controllerData=RecordingSteps[PlayingStep-1].data; 

*/    
  }


  
  
}



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

  if (!RunEffectOn && !Playing) //IF NOT RUNNING LIGHT EFFECT
  {

    NormalLedEffect();  // Normal leds
/*
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

*/      
  }
  



// Light effect if no button pressed.


  ControlLightEffect();



  if (PreparingForRecording) // Searching for first button pressed to start recording
  {
//        PointerToDataOld=PointerToDataNew;
        boolean NoButtonPressed=true;

/*        
        for (int i = 0; i < sizeof controllerData; i++)  
        {
             if ((char)*PointerToDataOld++ != (char)*PointerToDataNew++) NoButtonPressed=true;
       
            
              byte a=(char)*PointerToDataOld;
              byte b=(char)*PointerToDataNew;


              Serial.print("(");              
              Serial.print(a,BIN);              
              Serial.print(") vs (");              
              Serial.print(b,BIN);              
              Serial.println(")");
                   
            Serial.print(b,BIN);  
        }        

*/        
//        Serial.println("--------------------------");


            NoButtonPressed=SameButtonsPressed(controllerData);


        
//        delay(500);


    #ifdef DEBUG  

Serial.print("[");
Serial.print(controllerData.triangleOn,BIN);
Serial.print(controllerData.circleOn,BIN);
Serial.print(controllerData.squareOn,BIN);
Serial.print(controllerData.crossOn,BIN);
Serial.print("]");

Serial.print("[");
Serial.print(LastRecordingData.triangleOn,BIN);
Serial.print(LastRecordingData.circleOn,BIN);
Serial.print(LastRecordingData.squareOn,BIN);
Serial.print(LastRecordingData.crossOn,BIN);
Serial.println("]");

  #endif




/*
Serial.print(controllerData.l1On,BIN);
Serial.print(controllerData.l2On,BIN);
Serial.print(controllerData.l3On,BIN);
Serial.print(controllerData.r1On,BIN);
Serial.print(controllerData.r2On,BIN);
Serial.print(controllerData.r3On,BIN);
Serial.print(controllerData.dpadLeftOn,BIN);
Serial.print(controllerData.dpadUpOn,BIN);
Serial.print(controllerData.dpadRightOn,BIN);
Serial.print(controllerData.dpadDownOn,BIN);
Serial.print(controllerData.MinusOn,BIN);
Serial.print(controllerData.PlusOn,BIN);
Serial.print(controllerData.HomeOn,BIN);
Serial.print(controllerData.leftStickX,BIN);
Serial.print(controllerData.leftStickY,BIN);
Serial.print(controllerData.rightStickX,BIN);
Serial.print(controllerData.rightStickY,BIN);
Serial.println("]");
*/


        
        if (!NoButtonPressed) 
        {

          #ifdef DEBUG  
          Serial.println("Detected first button pressed");
          #endif
          

          Recording=true;
          PreparingForRecording=false;
//          SaveButtonsPressed(controllerData);   //Avoid first press button
//          return controllerData;
          
        }

/*        
#ifdef DEBUG  
        else
          Serial.println("NOOO Detected first button pressed");
#endif
*/                  

                  
//        PointerToDataOld=PointerToDataNew;         
  }



  if ( Recording )
  {

//Deactivating Light effect
/*
          CountDownNoPushEffect=0;  //reseting time.
          RotateSpeed=RotateSpeedInitial; 
          RunEffectOn=true;

//Blinking leds

             BlinkTime+=(millis()-LastTime);
             if ( BlinkTime > 1000000/2 )
             {
                Serial.print(BlinkTime);
                Serial.print(" ");
                Serial.print(LastTime);
                Serial.print(" ");
                Serial.println(millis());
                
                LastTime=millis();
                BlinkTime=0;
                for (int i=0;i<12;++i)
                {
                  if (digitalRead(myLedPins[i]) == LOW )
                   digitalWrite(myLedPins[i],HIGH);
                  else
                   digitalWrite(myLedPins[i],LOW);
                }
             }
*/
/*
            NoButtonPressed=SameButtonsPressed( controllerData);    
            boolean ChangeButton=false;
            for (int i = 0; i < sizeof controllerData; i++)
            {             
              if ((char)*PointerToDataOld++ != (char)*PointerToDataNew++) 
              {
                ChangeButton=true;
              }
            }
*/

            if (!SameButtonsPressed( controllerData))  //Register new button pressed
            {

                    RecordingSteps[RecordingStep].data.triangleOn   = controllerData.triangleOn;
                    RecordingSteps[RecordingStep].data.circleOn     = controllerData.circleOn;
                    RecordingSteps[RecordingStep].data.squareOn     = controllerData.squareOn;
                    RecordingSteps[RecordingStep].data.crossOn      = controllerData.crossOn;
                    RecordingSteps[RecordingStep].data.l1On         = controllerData.l1On;
                    RecordingSteps[RecordingStep].data.l2On         = controllerData.l2On;
                    RecordingSteps[RecordingStep].data.l3On         = controllerData.l3On;
                    RecordingSteps[RecordingStep].data.r1On         = controllerData.r1On;
                    RecordingSteps[RecordingStep].data.r2On         = controllerData.r2On;
                    RecordingSteps[RecordingStep].data.r3On         = controllerData.r3On;
                    RecordingSteps[RecordingStep].data.dpadLeftOn   = controllerData.dpadLeftOn;
                    RecordingSteps[RecordingStep].data.dpadUpOn     = controllerData.dpadUpOn;
                    RecordingSteps[RecordingStep].data.dpadRightOn  = controllerData.dpadRightOn;
                    RecordingSteps[RecordingStep].data.dpadDownOn   = controllerData.dpadDownOn;
                    RecordingSteps[RecordingStep].data.MinusOn      = controllerData.MinusOn;
                    RecordingSteps[RecordingStep].data.PlusOn       = controllerData.PlusOn;
                    RecordingSteps[RecordingStep].data.HomeOn       = controllerData.HomeOn;

                    RecordingSteps[RecordingStep].data.leftStickX   = controllerData.leftStickX;
                    RecordingSteps[RecordingStep].data.leftStickY   = controllerData.leftStickY;
                    RecordingSteps[RecordingStep].data.rightStickX  = controllerData.rightStickX;
                    RecordingSteps[RecordingStep].data.rightStickY  = controllerData.rightStickY;

#ifdef DEBUG  
                    Serial.print("Record data: ");
                    Serial.print(RecordingStep);

                    Serial.print(" [");
                    Serial.print(RecordingSteps[RecordingStep].data.triangleOn,BIN);
                    Serial.print(RecordingSteps[RecordingStep].data.circleOn,BIN);
                    Serial.print(RecordingSteps[RecordingStep].data.squareOn,BIN);
                    Serial.print(RecordingSteps[RecordingStep].data.crossOn,BIN);
                    Serial.print("] ");
#endif


/*              
                const byte *MyPointerToDataHist = (byte*) &RecordingSteps[Recording].data;              
                MyPointerToDataHist=PointerToDataNew;
*/

                if (RecordingStep==0) //first step
                {
                  startTimeComboRecord=millis(); 
                }
  
                RecordingSteps[RecordingStep].Timestamp=millis()-startTimeComboRecord;

#ifdef DEBUG 
                Serial.println(RecordingSteps[RecordingStep].Timestamp);
                
#endif                
                               
                RecordingStep++;
 
            }


    }

  


  SaveButtonsPressed(controllerData);    
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

  buttonPushed=AnyButtonPressed();

/*  
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
*/

  currentTime=millis();

  if (buttonPushed) 
  {
    CountDownNoPushEffect=0;  //reseting time.
    RotateSpeed=RotateSpeedInitial; 
    RunEffectOn=false;
  }
  else
  CountDownNoPushEffect += (currentTime-oldTime);  // +Delta

  if (CountDownNoPushEffect>CountDownNoPushEffectConst)
  {
    RunEffect();
    RunEffectOn=true;
  }
  
  
  oldTime=currentTime;
   
  
}


void RunEffect()
{
    currentTime=millis();

    currentTimeLedsRotate += (currentTime-oldLedsTime);
    if (  currentTimeLedsRotate > RotateSpeed )
    {

      RotateSpeed-=5;
      if (RotateSpeed<0) RotateSpeed=40;

      currentTimeLedsRotate=0;  // Reset. Wait for next iteration.

//      TurnOffAllLeds();
            
      if (LightOnOff) digitalWrite(myLedPins[PointerLed],HIGH);
      else
      digitalWrite(myLedPins[PointerLed],LOW);
      
      ++PointerLed;
      if (PointerLed> 11) 
      {
        PointerLed=0;
        LightOnOff=!LightOnOff;    
      }
      
    }

    oldLedsTime=currentTime;
}


boolean AnyButtonPressed()
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

  return buttonPushed;
}


boolean SameButtonsPressed(dataForController_t controllerData)
{
  boolean Same=true;


  if ( controllerData.triangleOn != LastRecordingData.triangleOn ) Same=false;
  if ( controllerData.circleOn != LastRecordingData.circleOn ) Same=false;
  if ( controllerData.squareOn != LastRecordingData.squareOn ) Same=false;
  if ( controllerData.crossOn != LastRecordingData.crossOn ) Same=false;

  if ( controllerData.l1On != LastRecordingData.l1On ) Same=false;
  if ( controllerData.l2On != LastRecordingData.l2On ) Same=false;
  if ( controllerData.l3On != LastRecordingData.l3On ) Same=false;
  if ( controllerData.r1On != LastRecordingData.r1On ) Same=false;
  if ( controllerData.r2On != LastRecordingData.r2On ) Same=false;
  if ( controllerData.r3On != LastRecordingData.r3On ) Same=false;
  if ( controllerData.dpadLeftOn != LastRecordingData.dpadLeftOn ) Same=false;
  if ( controllerData.dpadUpOn != LastRecordingData.dpadUpOn ) Same=false;
  if ( controllerData.dpadRightOn != LastRecordingData.dpadRightOn ) Same=false;
  if ( controllerData.dpadDownOn != LastRecordingData.dpadDownOn ) Same=false;
/*  
  if ( controllerData.MinusOn != LastRecordingData.MinusOn ) Same=false;
  if ( controllerData.PlusOn != LastRecordingData.PlusOn ) Same=false;
  if ( controllerData.HomeOn != LastRecordingData.HomeOn ) Same=false;
*/
  if ( controllerData.leftStickX != LastRecordingData.leftStickX ) Same=false;
  if ( controllerData.leftStickY != LastRecordingData.leftStickY ) Same=false;
  if ( controllerData.rightStickX != LastRecordingData.rightStickX ) Same=false;
  if ( controllerData.rightStickY != LastRecordingData.rightStickY ) Same=false;


  return Same;
}


void SaveButtonsPressed(dataForController_t controllerData)
{

  LastRecordingData.triangleOn=controllerData.triangleOn;
  LastRecordingData.circleOn=controllerData.circleOn;
  LastRecordingData.squareOn=controllerData.squareOn;
  LastRecordingData.crossOn=controllerData.crossOn;

  LastRecordingData.l1On = controllerData.l1On;
  LastRecordingData.l2On = controllerData.l2On;
  LastRecordingData.l3On = controllerData.l3On;
  LastRecordingData.r1On = controllerData.r1On;

  LastRecordingData.r2On = controllerData.r2On;
  LastRecordingData.r3On = controllerData.r3On;
  LastRecordingData.dpadLeftOn = controllerData.dpadLeftOn;
  LastRecordingData.dpadUpOn = controllerData.dpadUpOn;
  LastRecordingData.dpadRightOn = controllerData.dpadRightOn;
  LastRecordingData.dpadDownOn = controllerData.dpadDownOn;
  LastRecordingData.MinusOn = controllerData.MinusOn;
  LastRecordingData.PlusOn = controllerData.PlusOn;
  LastRecordingData.HomeOn = controllerData.HomeOn;

  LastRecordingData.leftStickX = controllerData.leftStickX;
  LastRecordingData.leftStickY = controllerData.leftStickY;
  LastRecordingData.rightStickX = controllerData.rightStickX;
  LastRecordingData.rightStickY = controllerData.rightStickY;

}



void NormalLedEffect()  // Normal leds
{

//  if ( Playing ) return;
  
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
  }


void WriteLedsFromPlaying(dataForController_t controllerData)
{
/*  
                    controllerData.triangleOn = RecordingSteps[PlayingStep].data.triangleOn;
                    controllerData.circleOn = RecordingSteps[PlayingStep].data.circleOn;
                    controllerData.squareOn = RecordingSteps[PlayingStep].data.squareOn;
                    controllerData.crossOn = RecordingSteps[PlayingStep].data.crossOn;
                    controllerData.l1On = RecordingSteps[PlayingStep].data.l1On;
                    controllerData.l2On = RecordingSteps[PlayingStep].data.l2On;
                    controllerData.l3On = RecordingSteps[PlayingStep].data.l3On;
                    controllerData.r1On = RecordingSteps[PlayingStep].data.r1On;
                    controllerData.r2On = RecordingSteps[PlayingStep].data.r2On;
                    controllerData.r3On = RecordingSteps[PlayingStep].data.r3On;
                    controllerData.dpadLeftOn = RecordingSteps[PlayingStep].data.dpadLeftOn;
                    controllerData.dpadUpOn = RecordingSteps[PlayingStep].data.dpadUpOn;
                    controllerData.dpadRightOn = RecordingSteps[PlayingStep].data.dpadRightOn;
                    controllerData.dpadDownOn = RecordingSteps[PlayingStep].data.dpadDownOn;  
                    controllerData.MinusOn = RecordingSteps[PlayingStep].data.MinusOn;
                    controllerData.PlusOn = RecordingSteps[PlayingStep].data.PlusOn;
                    controllerData.HomeOn = RecordingSteps[PlayingStep].data.HomeOn;

                    controllerData.leftStickX = RecordingSteps[PlayingStep].data.leftStickX;
                    controllerData.leftStickY = RecordingSteps[PlayingStep].data.leftStickY;
                    controllerData.rightStickX = RecordingSteps[PlayingStep].data.rightStickX;
                    controllerData.rightStickY = RecordingSteps[PlayingStep].data.rightStickY;
*/

                    if ( controllerData.triangleOn )  digitalWrite(Led_pin_X,HIGH);      else                   digitalWrite(Led_pin_X,LOW);  
                    if ( controllerData.circleOn )    digitalWrite(Led_pin_Y,HIGH);      else                   digitalWrite(Led_pin_Y,LOW);  
                    if ( controllerData.squareOn )    digitalWrite(Led_pin_A,HIGH);      else                   digitalWrite(Led_pin_A,LOW);  
                    if ( controllerData.crossOn )     digitalWrite(Led_pin_B,HIGH);      else                   digitalWrite(Led_pin_B,LOW);  

/*
                    if ( controllerData.dpadUpOn )  digitalWrite(Led_pin_X,LOW);      else                   digitalWrite(Led_pin_X,HIGH);  
                    if ( controllerData.dpadDownOn )    digitalWrite(Led_pin_Y,LOW);      else                   digitalWrite(Led_pin_Y,HIGH);  
                    if ( controllerData.dpadLeftOn )    digitalWrite(Led_pin_A,LOW);      else                   digitalWrite(Led_pin_A,HIGH);  
                    if ( controllerData.dpadRightOn )     digitalWrite(Led_pin_B,LOW);      else                   digitalWrite(Led_pin_B,HIGH);  
*/



                    if ( controllerData.l1On )    digitalWrite(Led_pin_SWITCH_L,HIGH);    else                digitalWrite(Led_pin_SWITCH_L, LOW);  
                    if ( controllerData.l2On )    digitalWrite(Led_pin_SWITCH_ZL,HIGH);   else                digitalWrite(Led_pin_SWITCH_ZL,LOW);  
//                    if ( controllerData.l3On )    digitalWrite(Led_pin_LEFT_3,HIGH);      else                digitalWrite(Led_pin_LEFT_3,   LOW);  

                    if ( controllerData.r1On )    digitalWrite(Led_pin_SWITCH_R,HIGH);    else                digitalWrite(Led_pin_SWITCH_R, LOW);  
                    if ( controllerData.r2On )    digitalWrite(Led_pin_SWITCH_ZR,HIGH);   else                digitalWrite(Led_pin_SWITCH_ZR,LOW);  
//                    if ( controllerData.r3On )    digitalWrite(Led_pin_RIGHT_3,HIGH);     else                digitalWrite(Led_pin_RIGHT_3,  LOW);  

                    if (controllerData.leftStickX< 128 ) digitalWrite(Led_LeftStick_Left_pin,HIGH); else digitalWrite(Led_LeftStick_Left_pin,LOW);
                    if (controllerData.leftStickX> 128 ) digitalWrite(Led_LeftStick_Right_pin,HIGH); else digitalWrite(Led_LeftStick_Right_pin,LOW);

                    if (controllerData.leftStickY< 128 ) digitalWrite(Led_LeftStick_Top_pin,HIGH); else digitalWrite(Led_LeftStick_Top_pin,LOW);
                    if (controllerData.leftStickY> 128 ) digitalWrite(Led_LeftStick_Down_pin,HIGH); else digitalWrite(Led_LeftStick_Down_pin,LOW);

int Led_LeftStick_Top_pin   =  48  ;
int Led_LeftStick_Right_pin =  49  ;
int Led_LeftStick_Down_pin  =  50  ;
int Led_LeftStick_Left_pin  =  51  ;
                    
                    
}
