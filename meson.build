project('libx52', 'C',
  license: 'GPL-2.0-only WITH Classpath-exception-2.0',
  version: configure_file('Version'))

# Internationalization
i18n = import('i18n')
# define GETTEXT_PACKAGE
add_project_arguments('-DGETTEXT_PACKAGE="libx52"', language:'C')
i18n.gettext(meson.project_name())

dep_systemd = dependency('systemd', required: false)
dep_libusb = dependency('libusb-1.0', required: true)
dep_evdev = dependency('libevdev', required: false)
dep_hidapi = dependency('hidapi-hidraw', required: false)
if not dep_hidapi.found()
  dep_hidapi = dependency('hidapi', required: true)
endif

dep_udev = dependency('udev', required: false)

doxygen_program = find_program('doxygen', required: false)

# Python 3.5 or greater is needed
pymod = import('python')
pyinstall = pymod.find_installation('python3')
pyversion = pyinstall.language_version().split('.')
assert(pyversion[1].to_int() >= 5, 'Require Python >= 3.5')

#######################################################################
# Internal dependencies
#######################################################################

# pinelog
sub_pinelog = subproject('pinelog', required: true)
dep_pinelog = sub_pinelog.get_variable('libpinelog_dep')

# inih
# Try to use system inih, otherwise fallback to using Git
dep_inih = dependency('inih', required: false)
if not dep_inih.found()
  sub_inih = subproject('inih', required: true)
  dep_inih = sub_inih.get_variable('inih_dep')
endif
