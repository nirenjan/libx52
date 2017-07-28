X52 Pro MFD library
===================

This is a library which communicates using libusb-1.0 with the Saitek X52 Pro
HOTAS. The library provides several functions to control the LEDs and MFD of
the above HOTAS.

# Supported functions

* Set the LED state of all LEDs
* Write text to each of the MFD lines
* Set the date/time on the MFD
* Make the clutch and hat LEDs blink
* Display the "SHIFT" indicator in the MFD

Note that when writing text to the MFD, the line length is limited to 16
characters. While you can pass a longer string, the library will only consider
the first 16 characters for writing to the display.

