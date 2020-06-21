Saitek X52 Controller Map
=========================

This page documents the HID Report format and how to decode the report to map
to axis/buttons for the Saitek X52.

> **Note:** I am not sure that the button indices match with real hardware. The
> indices are guessed by comparing the Test screen printed in the manuals for
> both the X52 and X52 Pro, and my own
> [documentation](x52pro_controller_map.md).  I also looked at the [source code
> of this project](https://github.com/xythobuz/Saitek-X52-PPM) to figure out
> possible mappings. Given that the above project only uses PID 0x0255, it is
> entirely possible that the report format for the X52 with PID 0x075c is
> different.

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
* 15 - Stick 4-way N
* 16 - Stick 4-way E
* 17 - Stick 4-way S
* 18 - Stick 4-way W
* 19 - Throttle 4-way N (pull up)
* 20 - Throttle 4-way E
* 21 - Throttle 4-way S (push down)
* 22 - Throttle 4-way W
* 23 - MODE 1
* 24 - MODE 2
* 25 - MODE 3
* 26 - FUNCTION
* 27 - START/STOP/Down
* 28 - RESET/Up
* 29 - CLUTCH (i)
* 30 - Mouse button - primary
* 31 - Mouse button - secondary
* 32 - Mouse wheel down
* 33 - Mouse wheel up

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
The X52 reports 14 bytes of data everytime a joystick event occurs.

The 15 bytes are laid out in little endian format as shown below:

* 11-bits of X axis position
* 11-bits of Y axis position
* 10-bits of twist axis position
* 8 bits of throttle position
* 8 bits of rotary X position
* 8 bits of rotary Y position
* 8 bits of slider position
* 34 bits of button information
* 1 bits of padding
* 4 bits of hat position
* 4 bits of thumbstick X position
* 4 bits of thumbstick Y position

A report would look like the following:

    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |  X axis data        |  Y axis data        |  Rz axis data     |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |   Throttle    |  Rx axis data | Ry axis data  | Slider data   |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    | Buttons 7-0   | Buttons 15-8  | Buttons 23-16 | Buttons 31-24 |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |  Hat  |///|Btn| MouseX| MouseY|
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
