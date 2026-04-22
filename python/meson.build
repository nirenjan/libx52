# Generate the paths module so that the Python scripts work
# exactly the same as the C code.

path_data = configuration_data()
path_data.set('PREFIX', get_option('prefix'))
path_data.set('SYSCONFDIR', get_option('prefix') / get_option('sysconfdir'))
path_data.set('LOCALSTATEDIR', get_option('prefix') / get_option('localstatedir'))
path_data.set('DATADIR', get_option('prefix') / get_option('datadir'))
path_data.set('SOCK_PATH', get_option('prefix') / get_option('localstatedir') / 'run' / 'x52d' / 'x52d.cmd')

path_data_py = configure_file(
  input: 'paths.py.in',
  output: 'paths.py',
  configuration: path_data
)
