project('libx52', 'C',
  license: 'GPL-2.0-only WITH Classpath-exception-2.0',
  version: '0.3.3',
  meson_version: '>=0.61')

dep_libusb = dependency('libusb-1.0', required: true)
dep_hidapi = dependency('hidapi-hidraw', required: false)
if not dep_hidapi.found()
  dep_hidapi = dependency('hidapi', required: true)
endif

if host_machine.system() == 'linux'
  dep_evdev = dependency('libevdev', required: false)
else
  # Create a dummy dependency
  dep_evdev = dependency('', required: false)
endif

dep_systemd = dependency('systemd', required: false)
dep_udev = dependency('udev', required: false)

dep_cmocka = dependency('cmocka', required: false)
if not dep_cmocka.found()
  dep_cmocka = disabler()
endif

doxygen_program = find_program('doxygen', required: false)

# pkgconfig module is needed
pkgconfig = import('pkgconfig')

# Python 3.5 or greater is needed
pymod = import('python')
python = pymod.find_installation('python3')
pyversion = python.language_version().split('.')
assert(pyversion[1].to_int() >= 5, 'Require Python >= 3.5')

#######################################################################
# config.h
#######################################################################
compiler = meson.get_compiler('c')
cdata = configuration_data()
cdata.set_quoted('PACKAGE', meson.project_name())
cdata.set_quoted('PACKAGE_BUGREPORT', 'https://github.com/nirenjan/libx52/issues')
cdata.set_quoted('PACKAGE_NAME', meson.project_name())
cdata.set_quoted('LOCALEDIR', get_option('prefix') / get_option('localedir'))
cdata.set_quoted('SYSCONFDIR', get_option('prefix') / get_option('sysconfdir'))
cdata.set_quoted('LOCALSTATEDIR', get_option('prefix') / get_option('localstatedir'))
cdata.set_quoted('PACKAGE_VERSION', meson.project_version())
cdata.set_quoted('VERSION', meson.project_version())
cdata.set10('ENABLE_NLS', not get_option('nls').disabled())
cdata.set10('HAVE_FUNC_ATTRIBUTE_NORETURN', compiler.has_function_attribute('noreturn'))
cdata.set10('HAVE_STRUCT_TM_TM_GMTOFF',
  compiler.has_member('struct tm', 'tm_gmtoff', prefix:'#include <time.h>'))

config_h = configure_file(
  input: 'config.h.meson',
  output: 'config.h',
  configuration: cdata
)

#######################################################################
# Internationalization
#######################################################################
i18n = import('i18n')
dep_intl = dependency('intl', required: false)
if not dep_intl.found() and host_machine.system() == 'darwin'
  brew = find_program('brew', required: false)
  if brew.found()
    brew_prefix = run_command(brew, '--prefix', 'gettext',
      check: true).stdout().strip()

    brew_inc = brew_prefix / 'include'
    brew_lib_dir = brew_prefix / 'lib'

    dep_intl = compiler.find_library('intl', dirs: brew_lib_dir)
    dep_intl = declare_dependency(dependencies: dep_intl, include_directories: brew_inc)
  endif
endif

# # define GETTEXT_PACKAGE
if not get_option('nls').disabled()
  add_project_arguments(
    '-DGETTEXT_PACKAGE="@0@"'.format(meson.project_name()),
    language:'C')

  subdir('po')
endif

add_project_arguments('-isystem', meson.current_source_dir() / 'sys', language: 'C')

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
# Use system inih
dep_inih = dependency('inih')

#######################################################################
# Shared libraries and programs
#######################################################################
# Includes
includes = include_directories('.', 'libx52', 'libx52io', 'libx52util', 'vkm')

subdir('libx52')
subdir('libx52io')
subdir('libx52util')
subdir('vkm')
subdir('bugreport')
subdir('cli')
subdir('joytest')
subdir('evtest')
subdir('daemon')
subdir('udev')

#######################################################################
# Documentation - doxygen
#######################################################################
if doxygen_program.found()
  doxyfile = configure_file(
    input: 'Doxyfile.in',
    output: 'Doxyfile',
    configuration: {
      'PACKAGE_NAME': meson.project_name(),
      'PACKAGE_VERSION': meson.project_version(),
      'abs_top_builddir': meson.project_build_root(),
      'abs_top_srcdir': meson.project_source_root(),
    }
  )

  custom_target('docs',
    depend_files: [doxyfile, 'DoxygenLayout.xml'],
    command: [doxygen_program],
    output: 'docs'
    )

  meson.add_install_script(
    'install-doxygen-docs.sh',
    get_option('datadir')  / 'doc' / meson.project_name(),
    get_option('mandir'),
  )
endif
