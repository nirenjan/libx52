lipc_server_example = executable('lipc_server_example',
  'lipc_server_example.c',
  include_directories: [localipc_inc],
  link_with: lib_localipc,
  dependencies: [dep_localipc_threads],
  install: false,
)

lipc_client_example = executable('lipc_client_example',
  'lipc_client_example.c',
  include_directories: [localipc_inc],
  link_with: lib_localipc,
  dependencies: [dep_localipc_threads],
  install: false,
)
