vkm_version = '0.1.0'

vkm_files = files(
  'common.c'
)

vkm_stub_files = files(
  'stub.c'
)

if host_machine.system() == 'linux'
  vkm_dep = dep_evdev
  if dep_evdev.found()
    vkm_platform_files = files(
      'linux_evdev.c'
    )    
  else
    vkm_platform_files = vkm_stub_files
  endif
else
  vkm_dep = dependency('', required: false)
  vkm_platform_files = vkm_stub_files
endif

lib_vkm = library('vkm', vkm_files + vkm_platform_files,
  install: true,
  version: vkm_version,
  c_args: sym_hidden_cargs,
  dependencies: [vkm_dep, dep_intl],
  include_directories: [includes])

vkm_strerror_test = executable('vkm-strerror-test',
  'test_strerror.c',
  'common.c',
  build_by_default: false,
  dependencies: [dep_cmocka, dep_intl],
  include_directories: [includes],
)

test('vkm-strerror', vkm_strerror_test, protocol: 'tap')

if host_machine.system() == 'linux' and dep_evdev.found()
  dep_evdev_headers = dep_evdev.partial_dependency(compile_args: true, link_args: false)
  vkm_linux_evdev_test = executable('vkm_linux_evdev_test',
    files(
      'linux_evdev_test.c',
      'linux_evdev.c',
      'common.c',
    ),
    include_directories: [includes],
    dependencies: [dep_cmocka, dep_evdev_headers, dep_intl],
  )
  test('vkm_linux_evdev', vkm_linux_evdev_test)
endif

pkgconfig.generate(lib_vkm,
  name: 'vkm',
  description: 'Virtual keyboard/mouse interface library for injecting events into the host OS.',
  version: vkm_version)
