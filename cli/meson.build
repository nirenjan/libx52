# x52cli
executable('x52cli', 'x52_cli.c',
  install: true,
  include_directories: [includes],
  link_with: lib_libx52)

test_cli = executable('test-cli', 'x52_cli.c', 'test_x52_cli.c',
  build_by_default: false,
  c_args: ['-DX52_CLI_TESTING'],
  include_directories: [includes],
  dependencies: [dep_cmocka],
  )

test('test-cli', test_cli, protocol: 'tap')


