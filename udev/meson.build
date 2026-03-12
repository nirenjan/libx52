# udev rules
if dep_udev.found()
  udev_rules_dir = get_option('udev-rules-dir')
  if udev_rules_dir == ''
    udev_dir = dep_udev.get_variable('udevdir', default_value:'/lib/udev')
    udev_rules_dir = join_paths(udev_dir, 'rules.d')
  endif
  udev_file = configure_file(
    input: '60-saitek-x52-x52pro.rules.in',
    output: '60-saitek-x52-x52pro.rules',
    configuration: {'input_group': get_option('input-group')}
  )
  install_data(udev_file, install_dir: udev_rules_dir)
  meson.add_install_script('install-hook.sh')
endif
