# Arduino Timer with DF Robot (or D1 Robot) LCD Keypad Shield

This is a simple implementation of a timer clock with Arduino and the DF Robot (or the Chinese counterpart, D1 Robot)
LCD Keypad Shield.

The software is licensed under Creative Commons CC BY 4.0 as described at: https://creativecommons.org/licenses/by/4.0/

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

Power the assembly using your power source.

## How it works

There are basically three operation modes for this timer clock:

1) **Clock set mode**: in this mode of operation, you have to use the
shield keypad to set the timer clock. The cursor on the display shows
which value will be adjusted (whether it is the seconds, the minutes, 
or the hours). Press Up key to increase, and press Down key to decrease
the value. Left key navigates to the left (seconds -> minutes -> hours),
and Right key, to the right (hours -> minutes -> seconds). Select key
moves the timer clock to the second mode of operation.

2) **Timer mode**: once you pressed the Select button in the previous mode of
operation, the timer clock starts running. When the timer reaches 0h 0m 0s,
it is moved to the next mode of operation.

3) **Alert mode**: when the timer clock exhausts, "Timer exhausted" will be
shown on the LCD, and the buzzer will ring. The buzzer has to be connected
to the Arduino's pin 3 (connect the buzzer's red or + terminal to this pin) and
to the GND (buzzer's black or - terminal).

At any time, you can press the Reset button to go back to the first mode
of operation.

Thank you for using Arduino Timer!

Marcelo 