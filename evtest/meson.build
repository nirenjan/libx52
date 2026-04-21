# x52evtest
executable('x52evtest', ['ev_test.c', 'ev_denoise.c'],
  install: true,
  include_directories: includes,
  dependencies: [dep_intl],
  link_with: [lib_libx52io])

ev_denoise_test = executable('ev-denoise-test', ['ev_denoise_test.c', 'ev_denoise.c'],
  build_by_default: false,
  dependencies: [dep_cmocka],
  include_directories: includes,
)
test('ev-denoise', ev_denoise_test, protocol: 'tap')
