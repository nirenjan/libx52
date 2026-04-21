joytest_sources = files(
  'x52_test.c',
  'x52_test_args.c',
  'x52_test_mfd.c',
  'x52_test_led.c',
  'x52_test_clock.c',
)

joytest_libx52_mock = static_library('joytest-libx52-mock',
  'libx52_mock.c',
  include_directories: includes,
)

# x52test
exe_x52test = executable('x52test',
  joytest_sources,
  install: true,
  dependencies: [dep_intl],
  include_directories: includes,
  link_with: [lib_libx52])

exe_x52test_mock = executable('x52test-mock',
  joytest_sources,
  install: false,
  dependencies: [dep_intl],
  include_directories: includes,
  link_with: [joytest_libx52_mock],
)

sh = find_program('sh')
test('x52test-mock',
  sh,
  args: [
    '-c',
    'printf "\n" | env NO_DELAY=1 @0@'.format(exe_x52test_mock.full_path()),
  ],
  depends: exe_x52test_mock,
  protocol: 'exitcode',
  timeout: 60,
)

test('x52test-help', exe_x52test, args: ['help'], protocol: 'exitcode')
test('x52test-help-dashdash', exe_x52test, args: ['--help'], protocol: 'exitcode')
test('x52test-bad-arg', exe_x52test, args: ['not-a-test-id'], protocol: 'exitcode', should_fail: true)

x52test_args_test = executable('x52test-args-test',
  ['x52_test_args_test.c', 'x52_test_args.c'],
  build_by_default: false,
  dependencies: [dep_cmocka],
  include_directories: includes,
)
test('x52test-args', x52test_args_test, protocol: 'tap')
