#include "Timer.h"

#include <global.h>
#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

unsigned char highSpeed = CANSPEED_500;
unsigned char midSpeed = CANSPEED_250;
unsigned char lowSpeed = CANSPEED_125;

// states
bool ignitionActive = false;
bool doorsLocked = true;
bool mirrorsMoving = false;
bool mirrorsOpen = false;

Timer mirrorTransitionTimer;
int mirrorTransitionEvent;

const int ignitionPin = 4;

const int openMirrorMotorPin = 7;
const int closeMirrorMotorPin = 8;

void setup() {
  Serial.begin(9600);
  Serial.println("Starter");
  
  if(Canbus.init(lowSpeed))
    Serial.println("CAN Init ok");
  else
    Serial.println("Can't init CAN");
    
  pinMode(ignitionPin, INPUT_PULLUP);
  
  pinMode(openMirrorMotorPin, OUTPUT);
  pinMode(closeMirrorMotorPin, OUTPUT);
  
  digitalWrite(openMirrorMotorPin, HIGH);
  digitalWrite(closeMirrorMotorPin, HIGH);
}

void loop() {
  mirrorTransitionTimer.update();
  
  if (!mcp2515_check_message())
    return;
  
  tCAN message;

  if (mcp2515_get_message(&message))
    updateStatesFromMessage(&message);  

  updateMirrors();
}

// This needs to read from CAN data
void updateStatesFromMessage(tCAN *msg) {
  // do some stuff to check if message means ignition or doors etc
  int ignitionState = digitalRead(ignitionPin);

  ignitionActive = ignitionState == LOW;

  /*
    Message with id 528 appears to relate to door locking.
    Data block zero relates to door lock status
        64 - open
        32 - locked
        96 - deadlocked
    Data block one appears to be a counter incrememting every time an ation occurs.
  */
  if (msg->id == 528) {
    doorsLocked = msg->data[0] != 64;
  }
}


void updateMirrors() {
  if (ignitionActive && !mirrorsOpen) {    
    Serial.println("Opening mirrors as ignition on and and mirrors not open");
    openMirrors();
  } else if (ignitionActive){
    //Serial.println("Ignition on and mirrors open, returning");
    return;
  } else if (doorsLocked && mirrorsOpen) {
    Serial.println("doors locked and mirrors open. closing mirrors");
    closeMirrors();
  } else if (!doorsLocked && !mirrorsOpen) {
    Serial.println("doors open and mirrors closed. opening mirrors");
    openMirrors();
  }
}

void mirrorTransitionEnd(void *context) {
  stop();
}

// This is a hack because i dont know C++ well enough to execute mirrorTransitionEnd directly
void stop() {
  mirrorsMoving = false;
  Serial.println("transition end");

  digitalWrite(openMirrorMotorPin, HIGH);
  digitalWrite(closeMirrorMotorPin, HIGH);
}

// TODO: openMirrors and closeMirrors are virtually identitcal. This can be refactored to remove the duplication.
void openMirrors() {
  if (mirrorsOpen) {
    Serial.println("mirrors already open or opening");
    return;
  }
    
  mirrorsOpen = true;

  if (mirrorsMoving){
    Serial.println("mirrors already moving. Stopping");
    stop();
    mirrorTransitionTimer.stop(mirrorTransitionEvent);
  }
    
  mirrorsMoving = true;

  Serial.println("Opening");
   digitalWrite(closeMirrorMotorPin, LOW);
  
  mirrorTransitionTimer.after(5000, mirrorTransitionEnd, (void*)0);
}

void closeMirrors() {
  if (!mirrorsOpen) {
    Serial.println("mirrors already closed or closing");
    return;
  }
    
  mirrorsOpen = false;
  
  if (mirrorsMoving){
    Serial.println("mirrors already moving. Stopping");
    stop();
    mirrorTransitionTimer.stop(mirrorTransitionEvent);
  }
    
  mirrorsMoving = true;  
  
  Serial.println("Closing");
  digitalWrite(openMirrorMotorPin, LOW);
  
  mirrorTransitionTimer.after(5000, mirrorTransitionEnd, (void*)0);
}
