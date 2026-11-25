evtest_sources = files(
  'ev_main.c',
  'ev_dispatch.c',
  'ev_denoise.c',
)

# Mock libx52io library for end-to-end testing of ev_main.c
evtest_libx52io_mock = static_library('evtest-libx52io-mock',
  'libx52io_mock.c',
  include_directories: includes,
  link_with: [lib_libx52io],
)

# x52evtest
executable('x52evtest', evtest_sources,
  install: true,
  include_directories: includes,
  dependencies: [dep_intl],
  link_with: [lib_libx52io])

# x52evtest-mock
exe_x52evtest_mock = executable('x52evtest-mock',
  evtest_sources,
  install: false,
  dependencies: [dep_intl],
  include_directories: includes,
  link_with: [evtest_libx52io_mock],
)

test('x52evtest-mock', exe_x52evtest_mock,
  protocol: 'exitcode',
  timeout: 10,
)

ev_denoise_test = executable('ev-denoise-test', ['ev_denoise_test.c', 'ev_denoise.c'],
  build_by_default: false,
  dependencies: [dep_cmocka],
  include_directories: includes,
)
test('ev-denoise', ev_denoise_test, protocol: 'tap')

ev_dispatch_test = executable('ev-dispatch-test', ['ev_dispatch_test.c', 'ev_dispatch.c', 'ev_denoise.c'],
  build_by_default: false,
  dependencies: [dep_cmocka, dep_intl, dep_hidapi],
  include_directories: [includes, include_directories('../libx52io')],
  link_with: [lib_libx52io],
)
test('ev-dispatch', ev_dispatch_test, protocol: 'tap')
