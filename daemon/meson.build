# x52d (dep_config_h: Meson build-config.h; private API is daemon/config.h)
libx52dcomm_version = '1.0.0'

libx52dcomm_sources = [
  'comm_client.c',
  'comm_internal.c'
  ]

install_headers('x52dcomm.h', subdir: meson.project_name())

lib_libx52dcomm = library('x52dcomm', libx52dcomm_sources,
  dependencies: [dep_intl, dep_config_h],
  version: libx52dcomm_version,
  c_args: sym_hidden_cargs,
  install: true,
  include_directories: includes)

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
  ]

dep_threads = dependency('threads')

# Comm sources are compiled into x52d (same as Autotools); libx52dcomm is only for x52ctl.
x52d_linkwith = [lib_libx52, lib_vkm, lib_libx52io]
x52d_deps = [dep_pinelog, dep_inih, dep_threads, dep_math, dep_intl, dep_config_h]
x52d_cflags = []

exe_x52d = executable('x52d', x52d_sources + libx52dcomm_sources,
  install: true,
  include_directories: includes,
  c_args: sym_hidden_cargs + x52d_cflags,
  dependencies: x52d_deps,
  link_with: x52d_linkwith)

exe_x52ctl = executable('x52ctl', 'daemon_control.c',
  install: true,
  dependencies: [dep_intl, dep_config_h],
  include_directories: includes,
  link_with: lib_libx52dcomm)

install_data('x52d.conf',
  install_dir: join_paths(get_option('sysconfdir'), 'x52d'))

test('daemon-communication', files('test_daemon_comm.py')[0],
  depends: [exe_x52d, exe_x52ctl], protocol: 'tap')

x52d_mouse_test_sources = ['mouse_test.c', 'mouse.c']
x52d_mouse_test = executable('x52d-mouse-test', x52d_mouse_test_sources,
  include_directories: includes,
  dependencies: [dep_pinelog, dep_cmocka, dep_intl, dep_math, dep_config_h])

test('x52d-mouse-test', x52d_mouse_test, protocol: 'tap')

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
