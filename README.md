BetaSketcher
============

This is an Arduino sketch that uses the joystick and 8x8 LED matrix on
the Digispark Pro "Beta Shield" to provide an "etch-a-sketch" like
experience.

The joystick button applies paint. The joystick moves. Placing your
finger over the light sensor on the board will clear your drawing.

The interesting technical achievement of this sketch is the use of
Binary Angle Modulation to drive the LEDs in the matrix at varying
intesities simultaneously. This is all in the BAM library is included
in this sketch and can be extracted for other projects that would like
to be able to display "shades of red" on the 8x8 array.

You can learn more about Binary Angle Modulation here:
http://www.batsocks.co.uk/readme/art_bcm_3.htm
