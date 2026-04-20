libx52io_version = '1.1.0'

libx52io_files = files(
  'core.c',
  'axis.c',
  'parser.c',
  'strings.c',
  'device.c',
)

lib_libx52io = library('x52io', libx52io_files,
  install: true,
  version: libx52io_version,
  c_args: sym_hidden_cargs,
  dependencies: [dep_hidapi, dep_intl],
  include_directories: [includes])

pkgconfig.generate(lib_libx52io,
  name: 'libx52io',
  description: 'Linux/Unix library to read and parse X52 input',
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

test_io_strings = executable('test-io-strings', 'test_io_strings.c', libx52io_files,
  build_by_default: false,
  dependencies: [dep_cmocka, dep_hidapi, dep_intl],
  include_directories: [includes],
)
test('test-io-strings', test_io_strings, protocol: 'tap')


