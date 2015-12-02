X52Pro Vendor API
=================

The X52Pro has multiple LED switches, one multifunction display with
a 3x16 character display, a clock with 3 timezone offsets and a date
display. In addition, the MFD has a mode indicator which is controlled
by the mode selector switch on the joystick and a shift indicator
which is controlled by software.

To change the status of these fields, a USB control message must be
sent to the X52Pro device. The request type must be a Vendor type
and sent to the Device endpoint. The request must be 0x91 while the
length must be zero. Value and Index fields are dependent on the
function being controlled

LEDs
====
The X52Pro has the following LEDs indexed as follows:

* 1 - Launch LED (Green when safety is closed, Red when open)
* 2 - A button Red LED
* 3 - A button Green LED
* 4 - B button Red LED
* 5 - B button Green LED
* 6 - D button Red LED
* 7 - D button Green LED
* 8 - E button Red LED
* 9 - E button Green LED
* 10 - Toggle 1 Red LED
* 11 - Toggle 1 Green LED
* 12 - Toggle 2 Red LED
* 13 - Toggle 2 Green LED
* 14 - Toggle 3 Red LED
* 15 - Toggle 3 Green LED
* 16 - POV switch Red LED
* 17 - POV switch Green LED
* 18 - Clutch (i) button Red LED
* 19 - Clutch (i) button Green LED
* 20 - Throttle LED

To change the status of an LED, the index must be 0xb8, while
the value must be as follows.

     1         1
     5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    | LED Num       |0|0|0|0|0|0|0|S|
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    LED Num - as per the index list above.
    S - 1 to turn on, 0 to turn off.

Brightness
==========
The display brightness of the LEDs and MFD can be controlled
separately. All LEDs will be adjusted to the same brightness
level.

To adjust the brightness of the LEDs, the index must be 0xb2.
For the MFD, the index must be 0xb1.

The value must be between 0x00 and 0x80 with higher values indicating
greater brighness.

MFD Text
========
The MFD has 3 lines of 16 characters each. Each line can be written
to separately.

In order to write a new string, the line must first be cleared.
This can be done by writing a value of 0 with the index as follows:

* Line 1 - 0xd9
* Line 2 - 0xda
* Line 3 - 0xdc

Strings are then written in groups of 2 characters in little endian
order. That is, in order to write the characters ABCD to the display,
the packets must first be sent with 0x4241 in the value, and then 0x4443
in the value. Each subsequent write will append the value to the string
and writes that exceed 16 characters will be ignored.

To append two characters to the line, use the following indices:

* Line 1 - 0xd1
* Line 2 - 0xd2
* Line 3 - 0xd4

Refer to the character map for more information.

Shift Indicator
===============

To change the status of the shift indicator on the MFD, use index 0xfd
with value 0x51 to turn on the shift indicator and 0x50 to turn off.

Blink Clutch & Hat LEDs
=======================

To change the status of the blink module, use index 0xb4 with value
0x51 to start blinking the LEDs and 0x50 to stop blinking. You cannot
adjust the POV LED directly.

Clock
=====

The MFD has a clock display that supports displaying up to 3 timezones
in both 12 and 24 hour formats. The base clock (clock 1) must be set
using an absolute time in hours and minutes, while the secondary and
tertiary clocks are programmed using offsets from the base clock. Also,
the X52Pro does not have specific hardware to automatically update
the time, so the host is responsible for updating the time values.

To update the primary clock, use index 0xc0 and the value as follows.

     1         1
     5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |M|    Hour     |    Minute     |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    M - 1 for 24 hour format, 0 for 12 hour format
    Hour in 24 hour format - 0 to 23
    Minute from 0 to 59

Updating the secondary and tertiary clocks use the same value as given
below, but different indices, the secondary clock uses index 0xc1 while
the tertiary clock uses index 0xc2.

     1         1
     5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |M|0|0|0|0|I|      Offset       |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

    M - 1 for 24 hour format, 0 for 12 hour format
    I - 1 if negative offset from primary clock, 0 if positive offset
    Offset in minutes from base clock.

Note that if you give hour and minute values outside their respective
ranges, they will be stored as is, but for display purposes will be
displayed as HH (mod 24) and MM (mod 60). So, if you program a primary
clock of 12:61, and a secondary offset of -2, the primary clock will
display 12:01, but the secondary clock will display 12:59. Also, the
modulus operation is not a true modulus, but varies. Description of
the operation is outside the scope of this document.

Date
====

The MFD can also display a single date. Although the display treats
the values in the control messages in DD.MM.YY format, tweaking the
control messages can give you other formats. However, note that the
display itself limits the values that can be shown. DD & MM can
display values from 0 to 39, while YY can display values from 0 to
63.

To update date and month, use the index 0xc4 with the value as follows.

     1         1
     5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |    Month      |     Date      |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

To update year, use the index 0xc8 with the year as the value.
