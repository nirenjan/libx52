project('localipc', 'C',
  version: '0.1.0',
  license: 'GPL-2.0-only WITH Classpath-exception-2.0',
  meson_version: '>=0.61')

pkgconfig = import('pkgconfig')
compiler = meson.get_compiler('c')

sym_hidden_cargs = []
if compiler.has_argument('-fvisibility=hidden')
  sym_hidden_cargs = ['-fvisibility=hidden']
endif

dep_localipc_threads = dependency('threads')

localipc_inc = include_directories('include')

lib_localipc_sources = files('lipc_crc32.c', 'lipc_packet.c', 'lipc_dispatch.c', 'lipc_socket.c')

dep_cmocka = dependency('cmocka', required: false)

lib_localipc = library('localipc', lib_localipc_sources,
  version: meson.project_version(),
  install: true,
  c_args: sym_hidden_cargs,
  dependencies: [dep_localipc_threads],
  include_directories: [localipc_inc],
)

install_headers(
  'include/localipc/lipc.h',
  install_dir: get_option('includedir') / 'localipc',
)

pkgconfig.generate(lib_localipc,
  name: 'localipc',
  description: 'Framed local IPC protocol library (stream-safe packets, CRC, dispatch).',
  version: meson.project_version(),
)

liblocalipc_dep = declare_dependency(
  link_with: lib_localipc,
  include_directories: localipc_inc,
)

if dep_cmocka.found()
  lipc_packet_test = executable('lipc_packet_test', 'lipc_packet_test.c',
    include_directories: localipc_inc,
    link_with: lib_localipc,
    dependencies: [dep_localipc_threads, dep_cmocka],
  )
  test('lipc_packet', lipc_packet_test, protocol:'tap')

  lipc_dispatch_test = executable('lipc_dispatch_test', 'lipc_dispatch_test.c',
    include_directories: localipc_inc,
    link_with: lib_localipc,
    dependencies: [dep_localipc_threads, dep_cmocka],
  )
  test('lipc_dispatch', lipc_dispatch_test, protocol:'tap')
endif

if get_option('lipc_examples')
  subdir('examples')
endif
