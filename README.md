# Arduino Timer using DF Robot (or D1 Robot) LCD Keypad Shield

This is a simple implementation of a timer clock with Arduino and the DF Robot (or the Chinese counterpart, D1 Robot)
LCD Keypad Shield.

The software is licensed under Creative Commons CC BY 4.0 as described at: https://creativecommons.org/licenses/by/4.0/

## Acknowledgement

Thank you for your interest in the Arduino Timer!

Special thanks to Selina, my mom, who encouraged me to finish this gift in time for her 70th birthday party.

## Required Material

* Arduino Duemilanove, Diecimila, or Uno
* DF Robot (or D1 Robot) LCD Keypad Shield
* 5-volt active buzzer
* Power source for the assembly

## Instructions

Mount the shield on the Arduino board.

Set the contrast trimpot so you can see the characters on the LCD
display. The trimpot is the blue thing at the upper left corner of the 
shield, and has a tiny screw, which is its cursor.

I had to turn it about 9-10 times until I could read the display!

Upload ~/src/arduinoTimer/arduinoTimer.ino to your Arduino board.

Power up the assembly, using your power source.

## User's guide

There are basically three modes of operation for this timer clock:

1) **Clock set mode**: in this mode of operation, you have to use the
shield keypad to set the timer clock. The underscore cursor on the display shows
which value will be adjusted (whether it is the seconds, the minutes, 
or the hours digits). Press Up key to increase, and press Down key to decrease
the value. Left key navigates to the left (seconds -> minutes -> hours),
and Right key, to the right (hours -> minutes -> seconds). Select key
moves the timer clock to the second mode of operation.

2) **Timer mode**: once you press the Select button at Mode 1, the timer clock will start 
running. When the timer reaches 0h 0m 0s, it will be moved to the next mode of operation.

3) **Alert mode**: when the timer exhausts at Mode 2, "Timer exhausted" will be
shown on the LCD, and the buzzer will ring. The buzzer has to be connected
to the Arduino's Pin 3 (connect the buzzer's red or + terminal to this pin) and
to the GND Pin (buzzer's black or - terminal). Some soldering skill will help.

At any time, you can press the Reset button to go back to Mode 1.
