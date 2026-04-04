libx52_version = '2.4.2'

libx52_files = files(
  'control.c',
  'core.c',
  'date_time.c',
  'mfd_led.c',
  'strerror.c',
  'stringify.c',
)

lib_libx52 = library('x52', libx52_files,
  install: true,
  version: libx52_version,
  c_args: sym_hidden_cargs,
  dependencies: [dep_libusb, dep_intl],
  include_directories: [includes])

install_headers('libx52.h', subdir: meson.project_name())
pkgconfig.generate(lib_libx52,
  name: 'libx52',
  description: 'Linux/Unix library to control Saitek X52/X52Pro joystick extended functionality.',
  version: libx52_version)

# Unit tests for libx52
libx52_string_test = executable('libx52-string-test',
  'test_strings.c',
  'stringify.c',
  'strerror.c',
  build_by_default: false,
  dependencies: [dep_cmocka, dep_intl],
  link_with: [lib_libx52],
  include_directories: [includes, lib_libx52.private_dir_include()],
  )

test('libx52-string-test', libx52_string_test, protocol: 'tap')

test_gen_script = files('x52_test_gen.py')[0]

libx52_test_file = custom_target('libx52-test',
  build_by_default: false,
  depend_files: ['x52_test_gen.py', 'x52_tests.json'],
  command: [python, test_gen_script,  '@INPUT@'],
  capture: true,
  input: 'x52_tests.json',
  output: 'test_libx52.c'
  )

libx52test = executable('libx52test', libx52_test_file, libx52_files,
  c_args: ['-Dlibusb_control_transfer=__wrap_libusb_control_transfer'],
  dependencies: [dep_cmocka, dep_libusb, dep_intl],
  link_with: [lib_libx52],
  build_by_default: false,
  include_directories: [includes, lib_libx52.private_dir_include()],
  )

test('libx52test', libx52test, protocol: 'tap')
