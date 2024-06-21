project('libx52', 'C',
  license: 'GPL-2.0-only WITH Classpath-exception-2.0',
  version: '0.3.2')

# Internationalization
i18n = import('i18n')
# # define GETTEXT_PACKAGE
if get_option('nls').enabled()
  add_project_arguments(
    '-DGETTEXT_PACKAGE="@0@"'.format(meson.project_name()),
    language:'C')

  subdir('po')
endif

dep_libusb = dependency('libusb-1.0', required: true)
dep_hidapi = dependency('hidapi-hidraw', required: false)
if not dep_hidapi.found()
  dep_hidapi = dependency('hidapi', required: true)
endif

dep_evdev = dependency('libevdev', required: false)

dep_systemd = dependency('systemd', required: false)
dep_udev = dependency('udev', required: false)

dep_cmocka = dependency('cmocka', required: false)

doxygen_program = find_program('doxygen', required: false)

# pkgconfig module is needed
pkgconfig = import('pkgconfig')

# Python 3.5 or greater is needed
pymod = import('python')
python = pymod.find_installation('python3')
pyversion = python.language_version().split('.')
assert(pyversion[1].to_int() >= 5, 'Require Python >= 3.5')

#######################################################################
# Version information
#######################################################################
compiler = meson.get_compiler('c')
compiler_version = run_command(compiler.cmd_array(), '--version',
  capture: true,
  check: true).stdout().split('\n')[0]

build_date = run_command('date', '+%Y-%m-%dT%H:%M:%S%z',
  capture: true,
  check: true).stdout().strip()

build_host = run_command('uname', '-a',
  capture: true,
  check: true).stdout().strip()

built_for = '@0@ @1@ @2@-endian'.format(
  host_machine.system(),
  host_machine.cpu(),
  host_machine.endian(),
)

git = find_program('git', required: false)
if git.found()
  vcs_describe = run_command(git, 'describe', '--dirty',
    capture: true,
    check: false).stdout().strip()
  if vcs_describe == ''
    vcs_describe = meson.project_version()
  endif
else
  vcs_describe = meson.project_version()
endif

version_data = configuration_data()
version_data.set_quoted('BUILD_VERSION', vcs_describe)
version_data.set_quoted('BUILD_DATE', build_date)
version_data.set_quoted('BUILD_HOST', build_host)
version_data.set_quoted('BUILD_COMPILER', compiler_version)
version_data.set_quoted('BUILD_TARGET', built_for)
version_data.set_quoted('BUILD_VERSION_INFO_IDENT', '$Id$')

version_info_h = configure_file(
  input: 'version-info.h.meson',
  output: 'version-info.h',
  configuration: version_data
)

#######################################################################
# config.h
#######################################################################
cdata = configuration_data()
cdata.set_quoted('PACKAGE', meson.project_name())
cdata.set_quoted('PACKAGE_BUGREPORT', 'https://github.com/nirenjan/libx52/issues')
cdata.set_quoted('PACKAGE_NAME', meson.project_name())
cdata.set_quoted('LOCALEDIR', get_option('localedir'))
cdata.set_quoted('SYSCONFDIR', get_option('sysconfdir'))
cdata.set_quoted('LOCALSTATEDIR', get_option('localstatedir'))
cdata.set_quoted('PACKAGE_VERSION', meson.project_version())
cdata.set_quoted('VERSION', meson.project_version())
cdata.set10('ENABLE_NLS', get_option('nls').enabled())
cdata.set10('HAVE_FUNC_ATTRIBUTE_NORETURN', compiler.has_function_attribute('noreturn'))
cdata.set10('HAVE_STRUCT_TM_TM_GMTOFF',
  compiler.has_member('struct tm', 'tm_gmtoff', prefix:'#include <time.h>'))


config_h = configure_file(
  input: 'config.h.meson',
  output: 'config.h',
  configuration: cdata
)
#
#######################################################################
# Internal dependencies
#######################################################################

# pinelog
pinelog_options = []
if dep_systemd.found() and not get_option('systemd-logs').disabled()
  # If systemd logs is enabled or auto, and systemd is found, then hide
  # the timestamps in log messages
  pinelog_options = ['show-date=false']
endif

sub_pinelog = subproject('pinelog', required: true,
  default_options: pinelog_options)
dep_pinelog = sub_pinelog.get_variable('libpinelog_dep')

# inih
# Try to use system inih, otherwise fallback to using Git
dep_inih = dependency('inih', required: false)
if not dep_inih.found()
  sub_inih = subproject('inih', required: true)
  dep_inih = sub_inih.get_variable('inih_dep')
endif

#######################################################################
# Shared libraries
#######################################################################
# Includes
includes = include_directories('.', 'libx52')

# libx52
libx52_files = files(
  'libx52/x52_control.c',
  'libx52/x52_core.c',
  'libx52/x52_date_time.c',
  'libx52/x52_mfd_led.c',
  'libx52/x52_strerror.c',
  'libx52/x52_stringify.c',
)

lib_libx52 = library('x52', libx52_files,
  install: true,
  version: '2.4.1',
  dependencies: [dep_libusb])

install_headers('libx52/libx52.h', subdir: meson.project_name())
pkgconfig.generate(lib_libx52)

# Unit tests for libx52
if dep_cmocka.found()
  libx52_string_test = executable('libx52-string-test',
    'libx52/test_strings.c',
    'libx52/x52_stringify.c',
    'libx52/x52_strerror.c',
    build_by_default: false,
    dependencies: [dep_cmocka],
    link_with: [lib_libx52],
    include_directories: [includes, lib_libx52.private_dir_include()],
    )

  test('libx52-string-test', libx52_string_test, protocol: 'tap')

  libx52_test_file = custom_target('libx52-test',
    build_by_default: false,
    depend_files: ['libx52/x52_test_gen.py', 'libx52/x52_tests.json'],
    command: [python, '@SOURCE_ROOT@/libx52/x52_test_gen.py',  '@INPUT@'],
    capture: true,
    input: 'libx52/x52_tests.json',
    output: 'test_libx52.c'
    )

  libx52test = executable('libx52test', libx52_test_file, libx52_files,
    c_args: ['-Dlibusb_control_transfer=__wrap_libusb_control_transfer'],
    dependencies: [dep_cmocka, dep_libusb],
    link_with: [lib_libx52],
    build_by_default: false,
    include_directories: [includes, lib_libx52.private_dir_include()],
    )

  test('libx52test', libx52test, protocol: 'tap')
endif

# libx52io
libx52io_files = files(
  'libx52io/io_core.c',
  'libx52io/io_axis.c',
  'libx52io/io_parser.c',
  'libx52io/io_strings.c',
  'libx52io/io_device.c',
)

lib_libx52io = library('x52io', libx52io_files,
  install: true,
  version: '1.0.0',
  dependencies: [dep_hidapi])

install_headers('libx52io/libx52io.h', subdir: meson.project_name())
pkgconfig.generate(lib_libx52io)

if dep_cmocka.found()
  test_axis = executable('test-axis', 'libx52io/test_axis.c', libx52io_files,
    build_by_default: false,
    dependencies: [dep_cmocka, dep_hidapi],
  )
  test('test-axis', test_axis, protocol: 'tap')

  test_parser = executable('test-parser', 'libx52io/test_parser.c', libx52io_files,
    build_by_default: false,
    dependencies: [dep_cmocka, dep_hidapi],
  )
  test('test-parser', test_parser, protocol: 'tap')
endif

# libx52util
util_char_map = custom_target('util-char-map',
  build_by_default: false,
  depend_files: ['libx52util/x52_char_map_gen.py', 'libx52util/x52_char_map.cfg'],
  command: [python, '@SOURCE_ROOT@/libx52util/x52_char_map_gen.py', '@INPUT@', '@OUTPUT@'],
  input: 'libx52util/x52_char_map.cfg',
  output: 'util_char_map.c')

lib_libx52util = library('x52util', util_char_map, 'libx52util/x52_char_map_lookup.c',
  include_directories: 'libx52util',
  install: true,
  version: '1.0.0',
  )

install_headers('libx52util/libx52util.h', subdir: meson.project_name())
pkgconfig.generate(lib_libx52util)

#######################################################################
# Programs
#######################################################################
# x52bugreport
exe_bugreport = executable('x52bugreport', 'bugreport/bugreport.c',
  install: true,
  extra_files: ['bugreport/bugreport.dox'],
  include_directories: 'libx52io',
  dependencies: [dep_libusb, dep_hidapi],
  link_with: [lib_libx52io])

# Test only to get code coverage
test('x52bugreport', exe_bugreport, protocol:'exitcode')

# x52cli
executable('x52cli', 'cli/x52_cli.c',
  install: true,
  include_directories: 'libx52',
  link_with: lib_libx52)

if dep_cmocka.found()
  test_cli = executable('test-cli', 'cli/x52_cli.c', 'cli/test_x52_cli.c',
    build_by_default: false,
    c_args: ['-DX52_CLI_TESTING'],
    include_directories: 'libx52',
    dependencies: [dep_cmocka],
    )

  test('test-cli', test_cli, protocol: 'tap')
endif

# x52test
executable('x52test',
  'joytest/x52_test.c',
  'joytest/x52_test_mfd.c',
  'joytest/x52_test_led.c',
  'joytest/x52_test_clock.c',
  install: true,
  include_directories: includes,
  link_with: [lib_libx52])

# x52evtest
executable('x52evtest', 'evtest/ev_test.c',
  install: true,
  include_directories: ['.', 'libx52io'],
  link_with: [lib_libx52io])

# x52d
libx52dcomm_sources = [
  'daemon/x52d_comm_client.c',
  'daemon/x52d_comm_internal.c'
  ]

install_headers('daemon/x52dcomm.h', subdir: meson.project_name())

lib_libx52dcomm = library('x52dcomm', libx52dcomm_sources)

x52d_sources = [
  'daemon/x52d_main.c',
  'daemon/x52d_config_parser.c',
  'daemon/x52d_config_dump.c',
  'daemon/x52d_config.c',
  'daemon/x52d_device.c',
  'daemon/x52d_client.c',
  'daemon/x52d_clock.c',
  'daemon/x52d_mouse.c',
  'daemon/x52d_notify.c',
  'daemon/x52d_led.c',
  'daemon/x52d_command.c',
  ]

dep_threads = dependency('threads')
x52d_linkwith = [lib_libx52, lib_libx52dcomm]
x52d_deps = [dep_pinelog, dep_inih, dep_threads]
x52d_cflags = []
if dep_evdev.found()
  x52d_sources += 'daemon/x52d_io.c'
  x52d_sources += 'daemon/x52d_mouse_evdev.c'
  x52d_cflags += '-DHAVE_EVDEV'
  x52d_linkwith += lib_libx52io
  x52d_deps += dep_evdev
endif


exe_x52d = executable('x52d', x52d_sources,
  install: true,
  include_directories: ['.', 'libx52', 'libx52io', 'libx52util'],
  c_args: x52d_cflags,
  dependencies: x52d_deps,
  link_with: x52d_linkwith)

executable('x52ctl', 'daemon/x52ctl.c',
  install: true,
  include_directories: ['.'],
  link_with: lib_libx52dcomm)

install_data('daemon/x52d.conf',
  install_dir: join_paths(get_option('sysconfdir'), 'x52d'))

test('daemon-communication', files('daemon/test_daemon_comm.py')[0],
  depends: exe_x52d, protocol: 'tap')

if dep_cmocka.found()
  x52d_mouse_test_sources = ['daemon/x52d_mouse_test.c', 'daemon/x52d_mouse.c']
  x52d_mouse_test = executable('x52d-mouse-test', x52d_mouse_test_sources,
    include_directories: ['.', 'libx52', 'libx52io'],
    dependencies: [dep_pinelog, dep_cmocka])

  test('x52d-mouse-test', x52d_mouse_test, protocol: 'tap')
endif

# udev rules
if dep_udev.found()
  if meson.version().version_compare('>= 0.58.0')
    udev_dir = dep_udev.get_variable('udevdir', default_value:'/lib/udev')
  else
    udev_dir = dep_udev.get_pkgconfig_variable('udevdir', default:'/lib/udev')
  endif
  udev_rules_dir = join_paths(udev_dir, 'rules.d')
  udev_file = configure_file(
    input: 'udev/60-saitek-x52-x52pro.rules.in',
    output: '60-saitek-x52-x52pro.rules',
    configuration: {'input_group': get_option('input-group')}
  )
  install_data(udev_file, install_dir: udev_rules_dir)
  meson.add_install_script('udev/install-hook.sh')
endif
