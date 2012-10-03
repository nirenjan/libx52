Axis
====
* 0 - X axis
* 1 - Y axis
* 2 - Twist axis
* 3 - Throttle axis
* 4 - Clutch button rotary (rotary X)
* 5 - E button rotary (rotary Y)
* 6 - Slider

Buttons
=======
*  0 - Trigger primary
*  1 - FIRE/LAUNCH
*  2 - A
*  3 - B
*  4 - C
*  5 - SHIFT
*  6 - D
*  7 - E
*  8 - T1 Up
*  9 - T1 Down
* 10 - T2 Up
* 11 - T2 Down
* 12 - T3 Up
* 13 - T3 Down
* 14 - Trigger secondary
* 15 - Mouse button - primary
* 16 - Mouse wheel down
* 17 - Mouse wheel up
* 18 - Mouse button - secondary
* 19 - Stick 4-way N
* 20 - Stick 4-way E
* 21 - Stick 4-way S
* 22 - Stick 4-way W
* 23 - Throttle 4-way N (pull up)
* 24 - Throttle 4-way E
* 25 - Throttle 4-way S (push down)
* 26 - Throttle 4-way W
* 27 - MODE 1
* 28 - MODE 2
* 29 - MODE 3
* 30 - CLUTCH (i)
* 31 - FUNCTION
* 32 - START/STOP
* 33 - RESET
* 34 - Pg Up
* 35 - Pg Dn
* 36 - Up
* 37 - Dn
* 38 - MFD SELECT

The function button toggles between the clock and stopwatch on
the MFD. If in stopwatch mode, the start/stop button starts or
stops the stopwatch. If already stopped, the start/stop button
will restart the stopwatch. The reset button will stop the
stopwatch, if running, and also reset the display to 00:00.

If the MFD is in clock mode, then the start/stop button cycles
backwards through the three on-board clock displays, while the
reset button cycles forward through the displays.

If the primary clock has not been set using the vendor API,
then the MFD will not display anything in clock mode. The
start/stop and reset buttons will also not change anything
on the display. Stopwatch mode is not affected by this.

Refer to the vendor API documentation for details on how to
configure the MFD display.

USB Report Format
=================
The X52 reports 15 bytes of data everytime a joystick event occurs.

The 15 bytes are laid out in little endian format as shown below:

10-bits of X axis position
10-bits of Y axis position
10-bits of twist axis position
2 bits of padding
8 bits of throttle position
8 bits of rotary X position
8 bits of rotary Y position
8 bits of slider position
39 bits of button information
1 bit of padding
4 bits of hat position
4 bits of padding
4 bits of thumbstick X position
4 bits of thumbstick Y position
