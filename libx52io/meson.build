libx52io_version = '1.0.0'

libx52io_files = files(
  'io_core.c',
  'io_axis.c',
  'io_parser.c',
  'io_strings.c',
  'io_device.c',
)

lib_libx52io = library('x52io', libx52io_files,
  install: true,
  version: libx52io_version,
  c_args: sym_hidden_cargs,
  dependencies: [dep_hidapi, dep_intl],
  include_directories: [includes])

install_headers('libx52io.h', subdir: meson.project_name())
pkgconfig.generate(lib_libx52io,
  name: 'libx52io',
  description: 'Linux/Unix library to read and parse X52 input',
  subdirs: meson.project_name(),
  version: libx52io_version,
)

test_axis = executable('test-axis', 'test_axis.c', libx52io_files,
  build_by_default: false,
  dependencies: [dep_cmocka, dep_hidapi, dep_intl],
  include_directories: [includes],
)
test('test-axis', test_axis, protocol: 'tap')

test_parser = executable('test-parser', 'test_parser.c', libx52io_files,
  build_by_default: false,
  dependencies: [dep_cmocka, dep_hidapi, dep_intl],
  include_directories: [includes],
)
test('test-parser', test_parser, protocol: 'tap')


