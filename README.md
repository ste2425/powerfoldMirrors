# powerfoldMirrors
Arduino based power folding mirrors module.

A module that should inegrate with the vehicles canbus system and trigger power fold mirrors when the car is locked or unlocked.

If the ignition is active it should open the mirrors then disable the system. 

It will target a 2010 Focus for its can messages.

Essentially the arduino will use a simple Hbridge to toggle a motor for 5 seconds in either direction. 

It expects the actual mirror mechanism to have switches that will stop the mirror should it reach the fully closed or fully open positions.

In the future it would me neat to add a switch to allow the mirrors to be manually opened or closed. It would be cool to also have puddle lights and to optionaly open the mirrors when the drivers dooor os opened. Not just when the vehicle is unlocked.