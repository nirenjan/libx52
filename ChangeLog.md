# Changelog

All notable changes to this project will be documented in this file.

The format is based upon [Keep a Changelog].

## [Unreleased]
### Added
- Connect/Disconnect methods in libx52. These allow for dynamically connecting
  or disconnecting from a supported joystick without having to reinitialize the
  library.
- Internationalization for the following:
    * libx52
    * x52test
- Doxygen generation of HTML documentation for libx52 methods.

### Changed
- libx52_init no longer fails when a supported joystick is not connected.

### Fixed
- Error reporting in x52cli and x52test commands.
- Handling of very large time_t values in `libx52_set_clock`
- Secondary and tertiary clock setting when primary clock is set to local time
  and local timezone is observing daylight savings time (summer time). See
  [#20](https://github.com/nirenjan/x52pro-linux/issues/20).

## [0.2.0] - 2020-04-14
### Changed
- `libx52_init` now returns a `libx52_error_code`, and returns the
  `libx52_device` pointer in an output parameter.
- All libx52 APIs now return a `libx52_error_code` indicating the error.
- libx52 now checks the version of libusb and calls the appropriate method
  to set logging level.
- x52test has an option to not sleep between consecutive calls to the libx52
  APIs.

### Fixed
- `libx52_write_time` handling of large timezone offsets.

## [0.1.2] - 2017-08-17
### Added
- Autotools based unit tests - tests run on Linux only
- libusb mock library for use by test programs
- License file and usage clarification
- Automatic builds on Ubuntu Trusty (14.04) with both GCC and clang on Travis
- Enhanced documentation for libx52
- Support for X52 (non-Pro) version
- New raw time and date APIs for libx52
- Support raw time and date commands in x52cli
- Unicode translation points for halfwidth CJK and Katakana symbols

### Changed
- Update Python character map generator to comply with PEP-8 guidelines

### Fixed
- Compilation on OSX

## [0.1.1] - 2016-05-06
### Added
- Manpage for x52cli
- Manpages for libx52 in RONN format
- Unicode translation points for Latin, Greek and mathematical symbols
- Travis-CI based automatic compilation

### Changed
- libx52 clock API will return -EAGAIN if no update is needed.
- x52test accepts a list of tests to run, defaulting to ALL
- UTF-8 parser rewritten in Python

## [0.1.0] - 2015-12-09
### Added
- Support for semantic LED names instead of numbers in libx52
- Simpler API to control clocks
- Add documentation for X52 design and USB interface
- Add CLI application to interface with libx52
- Add test application to test all aspects of libx52
- Add API to convert UTF-8 string to X52 character map

### Changed
- Migrate project to autotools

### Deprecated
- Mark kernel driver as proof-of-concept and unsuitable for production

## [0.0.2] - 2014-10-18
### Added
- Proposed design documentation for MFD pages and input mapping

### Fixed
- Kernel module compilation on Linux v3.5 and later

## 0.0.1 - 2012-10-25
### Added
- Kernel module for Saitek X52 Pro Joystick
- First release of userspace application


[Keep a Changelog]: http://keepachangelog.com/en/1.0.0/
[Semantic Versioning]: http://semver.org/spec/v2.0.0.html
[Unreleased]: https://github.com/nirenjan/x52pro-linux/compare/v0.2.0...HEAD
[0.2.0]: https://github.com/nirenjan/x52pro-linux/compare/v0.1.2...v0.2.0
[0.1.2]: https://github.com/nirenjan/x52pro-linux/compare/v0.1.1...v0.1.2
[0.1.1]: https://github.com/nirenjan/x52pro-linux/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/nirenjan/x52pro-linux/compare/v0.0.2...v0.1.0
[0.0.2]: https://github.com/nirenjan/x52pro-linux/compare/v0.0.1...v0.0.2
