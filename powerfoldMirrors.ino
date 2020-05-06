#include <Event.h>
#include <Timer.h>

#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

unsigned char highSpeed = CANSPEED_500;
unsigned char lowSpeed = CANSPEED_250;

// states
bool ignitionActive = false;
bool doorsLocked = true;
bool mirrorsMoving = false;
bool mirrorsOpen = false;

Timer mirrorTransitionTimer;
int mirrorTransitionEvent;

void setup() {
  Serial.begin(9600); // For debug use
  
  if(Canbus.init(highSpeed))  //Initialise MCP2515 CAN controller at the specified speed
    Serial.println("CAN Init ok");
  else
    Serial.println("Can't init CAN");
    
  delay(1000);

}

void loop() {
  if (!mcp2515_check_message())
    return;
  
  tCAN message;

  if (!mcp2515_get_message(&message))
    return;

  updateStatesFromMessage(&message);

  updateMirrors();

}

void updateStatesFromMessage(tCAN *msg) {
  // do some stuff to check if message means ignition or doors etc
}


void updateMirrors() {
  if (ignitionActive && !mirrorsOpen) {
    openMirrors();
  } else if (ignitionActive){
    return;
  } else if (doorsLocked && mirrorsOpen) {
    closeMirrors();
  } else if (!doorsLocked && !mirrorsOpen) {
    openMirrors();
  }
}
void openMirrors() {
  if (mirrorsOpen)
    return;
    
  mirrorsOpen = true;

  if (mirrorsMoving)
    mirrorTransitionTimer.stop(mirrorTransitionEvent);
    
  mirrorsMoving = true;

  // some logic to set correct pins
  
  mirrorTransitionTimer.after(5000, mirrorTransitionEnd, (void*)0);
}

void closeMirrors() {
  if (!mirrorsOpen)
    return;
    
  mirrorsOpen = false;
  
  if (mirrorsMoving)
    mirrorTransitionTimer.stop(mirrorTransitionEvent);
    
  mirrorsMoving = true;  
  
  // some logic to set correct pins
  
  mirrorTransitionTimer.after(5000, mirrorTransitionEnd, (void*)0);
}

void mirrorTransitionEnd(void *context) {
  mirrorsMoving = false;

  // some logic to stop mirrors moving
}
