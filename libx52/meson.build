libx52_files = files(
  'x52_control.c',
  'x52_core.c',
  'x52_date_time.c',
  'x52_mfd_led.c',
  'x52_strerror.c',
  'x52_stringify.c',
)

lib_libx52 = library('x52', libx52_files,
  install: true,
  version: '2.4.1',
  dependencies: [dep_libusb],
  include_directories: [includes])

install_headers('libx52.h', subdir: meson.project_name())
pkgconfig.generate(lib_libx52)

# Unit tests for libx52
libx52_string_test = executable('libx52-string-test',
  'test_strings.c',
  'x52_stringify.c',
  'x52_strerror.c',
  build_by_default: false,
  dependencies: [dep_cmocka],
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
  dependencies: [dep_cmocka, dep_libusb],
  link_with: [lib_libx52],
  build_by_default: false,
  include_directories: [includes, lib_libx52.private_dir_include()],
  )

test('libx52test', libx52test, protocol: 'tap')
