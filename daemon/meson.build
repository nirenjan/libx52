# x52d
libx52dcomm_sources = [
  'x52d_comm_client.c',
  'x52d_comm_internal.c'
  ]

install_headers('x52dcomm.h', subdir: meson.project_name())

lib_libx52dcomm = library('x52dcomm', libx52dcomm_sources,
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
  ]

dep_threads = dependency('threads')
x52d_linkwith = [lib_libx52, lib_libx52dcomm]
x52d_deps = [dep_pinelog, dep_inih, dep_threads]
x52d_cflags = []
if dep_evdev.found()
  x52d_sources += 'x52d_io.c'
  x52d_sources += 'x52d_mouse_evdev.c'
  x52d_cflags += '-DHAVE_EVDEV'
  x52d_linkwith += lib_libx52io
  x52d_deps += dep_evdev
endif

exe_x52d = executable('x52d', x52d_sources,
  install: true,
  include_directories: includes,
  c_args: x52d_cflags,
  dependencies: x52d_deps,
  link_with: x52d_linkwith)

executable('x52ctl', 'x52ctl.c',
  install: true,
  include_directories: includes,
  link_with: lib_libx52dcomm)

install_data('x52d.conf',
  install_dir: join_paths(get_option('sysconfdir'), 'x52d'))

test('daemon-communication', files('test_daemon_comm.py')[0],
  depends: exe_x52d, protocol: 'tap')

x52d_mouse_test_sources = ['x52d_mouse_test.c', 'x52d_mouse.c']
x52d_mouse_test = executable('x52d-mouse-test', x52d_mouse_test_sources,
  include_directories: includes,
  dependencies: [dep_pinelog, dep_cmocka])

test('x52d-mouse-test', x52d_mouse_test, protocol: 'tap')

