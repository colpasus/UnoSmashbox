
## UnoSmashbox 

UnoSmashbox allows to convert an Arduino Mega into a Nintendo Switch Smashbox controller with full functionality to play any arcade game but focused on the Super Smash Bros Ultimate game.

A Smashbox controller needs at least 23 buttons but in a future version I want to implement the possibility to create macros, so it is necessary to use Arduino Mega to not run out of input pins.

This project has been possible thanks to the amazing AlanChatham.
Unojoy project to understand the communication between the Mega 2560 chip and the ATmega8u2 USB chip, the modifications have been made to use bertrandom's project "snowball-thrower" that was created for the automatic farming machine in Zelda Breath of the Wild using an Arduino with USB libraries to communicate with Nintendo Switch.

This project is a mix between both projects to get an arduino Smashbox Joystick adapted to the Smash Bros Ultimate game for some skills that has been replicated digitally since we don't have an analog stick for slow runs and so on.

Enjoy!


## LUFA

Uses the LUFA library and reverse-engineering of the Pokken Tournament Pro Pad for the Wii U to enable custom fightsticks on the Switch System v3.0.0

### Wait, what?
On June 20, 2017, Nintendo released System Update v3.0.0 for the Nintendo Switch. Along with a number of additional features that were advertised or noted in the changelog, additional hidden features were added. One of those features allows for the use of compatible USB controllers on the Nintendo Switch, such as the Pokken Tournament Pro Pad.

Unlike the Wii U, which handles these controllers on a 'per-game' basis, the Switch treats the Pokken controller as if it was a Switch Pro Controller. Along with having the icon for the Pro Controller, it functions just like it in terms of using it in other games, apart from the lack of physical controls such as analog sticks, the buttons for the stick clicks, or other system buttons such as Home or Capture.
