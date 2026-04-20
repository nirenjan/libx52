#######################################################################
# build-config.h (Meson configuration; template is config.h.meson)
#######################################################################
sym_hidden_cargs = []
if compiler.has_argument('-fvisibility=hidden')
  sym_hidden_cargs = ['-fvisibility=hidden']
endif
cdata = configuration_data()
cdata.set_quoted('PACKAGE', meson.project_name())
cdata.set_quoted('PACKAGE_BUGREPORT', 'https://github.com/nirenjan/libx52/issues')
cdata.set_quoted('PACKAGE_NAME', meson.project_name())
cdata.set_quoted('LOCALEDIR', get_option('prefix') / get_option('localedir'))
cdata.set_quoted('SYSCONFDIR', get_option('prefix') / get_option('sysconfdir'))
cdata.set_quoted('LOCALSTATEDIR', get_option('prefix') / get_option('localstatedir'))
cdata.set_quoted('DATADIR', get_option('prefix') / get_option('datadir'))
cdata.set_quoted('PACKAGE_VERSION', meson.project_version())
cdata.set_quoted('VERSION', meson.project_version())
cdata.set10('ENABLE_NLS', not get_option('nls').disabled())
cdata.set10('HAVE_FUNC_ATTRIBUTE_NORETURN', compiler.has_function_attribute('noreturn'))
cdata.set10('HAVE_STRUCT_TM_TM_GMTOFF',
  compiler.has_member('struct tm', 'tm_gmtoff', prefix:'#include <time.h>'))

config_h = configure_file(
  input: 'build-config.h.meson',
  output: 'build-config.h',
  configuration: cdata
)

dep_config_h = declare_dependency(sources: config_h)


install_headers(
  'libx52/libx52.h',
  'libx52/libx52io.h',
  'libx52/libx52util.h',
  'libx52/x52dcomm.h',
  subdir: 'libx52'
)

install_headers(
  'vkm/vkm.h',
  subdir: 'vkm'
)
