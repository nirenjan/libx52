# x52d
libx52dcomm_version = '1.0.0'

libx52dcomm_sources = [
  'x52d_comm_client.c',
  'x52d_comm_internal.c'
  ]

install_headers('x52dcomm.h', subdir: meson.project_name())

lib_libx52dcomm = library('x52dcomm', libx52dcomm_sources,
  dependencies: [dep_intl],
  version: libx52dcomm_version,
  install: true,
  include_directories: includes)

x52d_sources = [
  'x52d_main.c',
  'x52d_config_parser.c',
  'x52d_config_dump.c',
  'x52d_config.c',
  'x52d_device.c',
  'x52d_client.c',
  'x52d_clock.c',
  'x52d_mouse.c',
  'x52d_notify.c',
  'x52d_led.c',
  'x52d_command.c',
  'x52d_io.c',
  'x52d_mouse_handler.c',
  ]

dep_threads = dependency('threads')
x52d_linkwith = [lib_libx52, lib_libx52dcomm, lib_vkm, lib_libx52io]
x52d_deps = [dep_pinelog, dep_inih, dep_threads, dep_intl]
x52d_cflags = []

exe_x52d = executable('x52d', x52d_sources,
  install: true,
  include_directories: includes,
  c_args: x52d_cflags,
  dependencies: x52d_deps,
  link_with: x52d_linkwith)

exe_x52ctl = executable('x52ctl', 'x52ctl.c',
  install: true,
  dependencies: [dep_intl],
  include_directories: includes,
  link_with: lib_libx52dcomm)

install_data('x52d.conf',
  install_dir: join_paths(get_option('sysconfdir'), 'x52d'))

test('daemon-communication', files('test_daemon_comm.py')[0],
  depends: [exe_x52d, exe_x52ctl], protocol: 'tap')

x52d_mouse_test_sources = ['x52d_mouse_test.c', 'x52d_mouse.c']
x52d_mouse_test = executable('x52d-mouse-test', x52d_mouse_test_sources,
  include_directories: includes,
  dependencies: [dep_pinelog, dep_cmocka, dep_intl])

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
