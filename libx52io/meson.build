libx52io_files = files(
  'io_core.c',
  'io_axis.c',
  'io_parser.c',
  'io_strings.c',
  'io_device.c',
)

lib_libx52io = library('x52io', libx52io_files,
  install: true,
  version: '1.0.0',
  dependencies: [dep_hidapi],
  include_directories: [includes])

install_headers('libx52io.h', subdir: meson.project_name())
pkgconfig.generate(lib_libx52io)

test_axis = executable('test-axis', 'test_axis.c', libx52io_files,
  build_by_default: false,
  dependencies: [dep_cmocka, dep_hidapi],
  include_directories: [includes],
)
test('test-axis', test_axis, protocol: 'tap')

test_parser = executable('test-parser', 'test_parser.c', libx52io_files,
  build_by_default: false,
  dependencies: [dep_cmocka, dep_hidapi],
  include_directories: [includes],
)
test('test-parser', test_parser, protocol: 'tap')


