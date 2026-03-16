# libx52util
libx52util_version = '1.0.2'
gen_script = files('x52_char_map_gen.py')[0]

util_char_map = custom_target('util-char-map',
  build_by_default: false,
  depend_files: ['x52_char_map_gen.py', 'x52_char_map.cfg'],
  command: [python, gen_script, '@INPUT@', '@OUTPUT0@', '@OUTPUT1@'],
  input: 'x52_char_map.cfg',
  output: ['util_char_map.c', 'x52_char_map.bin'])

lib_libx52util = library('x52util', util_char_map, 'x52_char_map_lookup.c',
  install: true,
  version: libx52util_version,
  include_directories: [includes],
  )

install_headers('libx52util.h', subdir: meson.project_name())
pkgconfig.generate(lib_libx52util,
  name: 'libx52util',
  description: 'Extra utility functions to manage X52 extended functionality',
  subdirs: meson.project_name(),
  version: libx52util_version,
)

libx52util_bmp_test = executable(
  'libx52util-bmp-test',
  'x52_char_map_test.c',
  build_by_default: false,
  include_directories: [includes, lib_libx52util.private_dir_include()],
  link_with: [lib_libx52util]
  )

test('libx52util-bmp-test', libx52util_bmp_test,
  protocol: 'tap',
  args: [util_char_map[1]])
