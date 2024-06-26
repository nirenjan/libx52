# libx52util
gen_script = files('x52_char_map_gen.py')[0]

util_char_map = custom_target('util-char-map',
  build_by_default: false,
  depend_files: ['x52_char_map_gen.py', 'x52_char_map.cfg'],
  command: [python, gen_script, '@INPUT@', '@OUTPUT@'],
  input: 'x52_char_map.cfg',
  output: 'util_char_map.c')

lib_libx52util = library('x52util', util_char_map, 'x52_char_map_lookup.c',
  install: true,
  version: '1.0.0',
  include_directories: [includes],
  )

install_headers('libx52util.h', subdir: meson.project_name())
pkgconfig.generate(lib_libx52util)
