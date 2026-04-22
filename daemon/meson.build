# x52d (dep_config_h: Meson build-config.h; private API is daemon/config.h)
config_defs = custom_target('config-defs',
  depend_files: ['x52d_map_config.py', 'x52d.conf'],
  input: [
    'x52d.conf',
    'config_registry.json'
  ],
  output: [
    'config-defs.h',
    'config-defs.c',
    'config_defs.py'
  ],
  command: [
    python, meson.current_source_dir() / 'x52d_map_config.py',
    '@INPUT0@', '@INPUT1@',
    '@OUTPUT0@', '@OUTPUT1@', '@OUTPUT2@'
  ])

module_defs = custom_target('module-defs',
  depend_files: ['x52d_gen_module.py', 'module_defs.py'],
  output: ['module-map.h', 'module-map.c'],
  command: [python, meson.current_source_dir() / 'x52d_gen_module.py',
    '@OUTPUT0@', '@OUTPUT1@'])

# Header only: ordering on module-map.h without compiling module-map.c per target.
dep_module_map_gen = declare_dependency(sources: module_defs[0])

# Full module_defs: name-id-map.c includes module-map.h (generated with the .c).
slib_comm_defs = static_library('x52dcommdefs',
  config_defs[1],
  module_defs,
  'name-id-map.c',
  )

libx52dcomm_version = '1.0.0'

libx52dcomm_sources = [
  'comm_client.c',
  'comm_internal.c'
  ]

root_includes = include_directories('..')

lib_libx52dcomm = library('x52dcomm', libx52dcomm_sources,
  dependencies: [dep_intl, dep_config_h, dep_module_map_gen],
  version: libx52dcomm_version,
  c_args: sym_hidden_cargs,
  install: true,
  include_directories: [includes, root_includes])

pkgconfig.generate(lib_libx52dcomm,
  name: 'x52dcomm',
  description: 'Client library for communicating with the x52d X52 daemon.',
  version: libx52dcomm_version,
)

x52d_sources = [
  'main.c',
  'config_parser.c',
  'config_dump.c',
  'config.c',
  'device.c',
  'client.c',
  'clock.c',
  'mouse.c',
  'notify.c',
  'led.c',
  'command.c',
  'io.c',
  'mouse_handler.c',
  'layout_usage_allowlist.c',
  'layout_load.c',
  'keyboard_layout.c',
  'crc32.c',
  ]

dep_threads = dependency('threads')

# Comm sources are compiled into x52d (same as Autotools); libx52dcomm is only for x52ctl.
x52d_linkwith = [lib_libx52, lib_vkm, lib_libx52io, slib_comm_defs]
x52d_deps = [dep_pinelog, dep_inih, dep_threads, dep_math, dep_intl, dep_config_h,
  dep_module_map_gen]
x52d_cflags = []

exe_x52d = executable('x52d', x52d_sources + libx52dcomm_sources,
  install: true,
  include_directories: [includes, root_includes],
  c_args: sym_hidden_cargs + x52d_cflags,
  dependencies: x52d_deps,
  link_with: x52d_linkwith)

exe_x52ctl = executable('x52ctl', 'daemon_control.c',
  install: true,
  dependencies: [dep_intl, dep_config_h, dep_module_map_gen],
  include_directories: [includes, root_includes],
  link_with: lib_libx52dcomm)

install_data('x52d.conf',
  install_dir: join_paths(get_option('sysconfdir'), 'x52d'))

us_x52l = custom_target(
  'us-x52l',
  input: files('../data/layouts/us.layout'),
  output: 'us.x52l',
  command: [
    python,
    join_paths(meson.project_source_root(), 'tools', 'x52compile_layout.py'),
    '@INPUT@',
    '@OUTPUT@',
  ],
  install: true,
  install_dir: join_paths(get_option('datadir'), 'x52d'))

test('daemon-communication', files('test_daemon_comm.py')[0],
  depends: [exe_x52d, exe_x52ctl], protocol: 'tap')

x52d_mouse_test_sources = ['mouse_test.c', 'mouse.c']
x52d_mouse_test = executable('x52d-mouse-test', x52d_mouse_test_sources,
  include_directories: [includes, root_includes],
  dependencies: [dep_pinelog, dep_cmocka, dep_intl, dep_math, dep_config_h,
    dep_module_map_gen])

test('x52d-mouse-test', x52d_mouse_test, protocol: 'tap')

layout_usage_allowlist_test = executable('layout-usage-allowlist-test',
  'layout_usage_allowlist_test.c',
  'layout_usage_allowlist.c',
  build_by_default: false,
  include_directories: [includes, root_includes],
  dependencies: [dep_cmocka, dep_config_h])

test('layout-usage-allowlist', layout_usage_allowlist_test, protocol: 'tap')

crc32_test = executable('crc32-test', 'crc32_test.c', 'crc32.c',
  build_by_default: false,
  include_directories: [includes, root_includes],
  dependencies: [dep_cmocka, dep_config_h])

test('crc32', crc32_test, protocol: 'tap')

layout_load_test = executable('layout-load-test',
  'layout_load_test.c',
  'layout_load.c',
  'layout_usage_allowlist.c',
  'crc32.c',
  build_by_default: false,
  include_directories: [includes, root_includes],
  dependencies: [dep_cmocka, dep_config_h])

test('layout-load', layout_load_test, protocol: 'tap')

pymod_daemon = import('python')
python_layout_test = pymod_daemon.find_installation('python3')
test('layout-usage-allowlist-sync', python_layout_test,
  args: [join_paths(meson.project_source_root(), 'tools', 'test_layout_allowlist_sync.py')],
  protocol: 'tap')

test('layout-compile-py', python_layout_test,
  args: [join_paths(meson.project_source_root(), 'tools', 'test_x52compile_layout.py')],
  protocol: 'tap')

# Install service file
if dep_systemd.found()
  systemd_system_unit_dir = get_option('systemd-unit-dir')
  if systemd_system_unit_dir == ''
    systemd_system_unit_dir = dep_systemd.get_variable(
      pkgconfig: 'systemd_system_unit_dir',
      default_value: '/lib/systemd/system')
  endif
  sed = find_program('sed')
  bindir_path = get_option('prefix') / get_option('bindir')
  sed_script = 's|%bindir%|' + bindir_path + '|g'

  systemd_service_file = configure_file(
    input: 'x52d.service.in',
    output: 'x52d.service',
    command: [sed, sed_script, '@INPUT@'],
    capture: true,
    install: true,
    install_dir: systemd_system_unit_dir
  )
endif
