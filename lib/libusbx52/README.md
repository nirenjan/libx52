LibUSB mocker library
=====================

This folder contains a convenience library to mock the API of libusb. This is
intended to be used as an LD_PRELOAD library when used by automated tests to
verify the library without needing actual hardware to verify the tests.

While a manual test using real hardware is valuable, running some automated
tests in an environment where the hardware is not available is equally valuable,
especially if the source code is changing frequently.

Note that the API exported by the mocker is limited to the API used by libx52,
as writing a complete USB simulator stack in software is not an easy job, nor is
it necessary for the purposes of this project.

