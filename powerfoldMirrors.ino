#include "Timer.h"

#include <defaults.h>
#include <global.h>

// states
bool ignitionActive = false;
bool doorsLocked = true;
bool mirrorsMoving = false;
bool mirrorsOpen = false;

Timer mirrorTransitionTimer;
int mirrorTransitionEvent;

const int openPin = 2;    
const int closePin = 3;
const int ignitionPin = 4;

const int openMirrorMotorPin = 7;
const int closeMirrorMotorPin = 8;

void setup() {
  Serial.begin(9600);
  Serial.println("Starter");
    
  pinMode(openPin, INPUT_PULLUP);
  pinMode(closePin, INPUT_PULLUP);
  pinMode(ignitionPin, INPUT_PULLUP);
  
  pinMode(openMirrorMotorPin, OUTPUT);
  pinMode(closeMirrorMotorPin, OUTPUT);
  
  digitalWrite(openMirrorMotorPin, HIGH);
  digitalWrite(closeMirrorMotorPin, HIGH);
}

void loop() {
  mirrorTransitionTimer.update();
  updateStatesFromMessage();

  updateMirrors();
}

// This needs to read from CAN data
void updateStatesFromMessage() {
  // do some stuff to check if message means ignition or doors etc
  int openState = digitalRead(openPin);
  int closeState = digitalRead(closePin);
  int ignitionState = digitalRead(ignitionPin);

  if (openState == LOW)
    doorsLocked = false;
  else if (closeState == LOW)
    doorsLocked = true;

  ignitionActive = ignitionState == LOW;
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
