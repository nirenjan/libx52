#######################################################################
# Version information
#######################################################################
compiler_version = run_command(compiler.cmd_array(), '--version',
  capture: true,
  check: true).stdout().split('\n')[0]

build_date = run_command('date', '+%Y-%m-%dT%H:%M:%S%z',
  capture: true,
  check: true).stdout().strip()

build_kernel = run_command('uname', '-sr',
  capture: true,
  check: true).stdout().strip()

build_arch = run_command('uname', '-mp',
  capture: true,
  check: true).stdout().strip()

build_os_version = run_command('uname', '-v',
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
version_data.set_quoted('BUILD_KERNEL', build_kernel)
version_data.set_quoted('BUILD_ARCH', build_arch)
version_data.set_quoted('BUILD_OS_VERSION', build_os_version)
version_data.set_quoted('BUILD_COMPILER', compiler_version)
version_data.set_quoted('BUILD_TARGET', built_for)
version_data.set_quoted('BUILD_VERSION_INFO_IDENT', '$Id$')

version_info_h = configure_file(
  input: 'version-info.h.meson',
  output: 'version-info.h',
  configuration: version_data
)

# x52bugreport
exe_bugreport = executable('x52bugreport', 'bugreport.c',
  install: true,
  include_directories: [includes],
  dependencies: [dep_libusb, dep_hidapi],
  link_with: [lib_libx52io])

# Test only to get code coverage
test('x52bugreport', exe_bugreport, protocol:'exitcode')
