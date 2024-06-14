project('pinelog', 'C',
	default_options: {'show-date': true, 'show-level': true, 'show-backtrace': true})

pinelog_cflags = [
	'-DPINELOG_SHOW_DATE=@0@'.format(get_option('show-date').to_int()),
	'-DPINELOG_SHOW_LEVEL=@0@'.format(get_option('show-level').to_int()),
	'-DPINELOG_SHOW_BACKTRACE=@0@'.format(get_option('show-backtrace').to_int()),
]

libpinelog = static_library('pinelog', 'pinelog.c', c_args: pinelog_cflags)

libpinelog_inc = include_directories('.')
libpinelog_dep = declare_dependency(
	include_directories: libpinelog_inc,
	link_with: libpinelog
)

prog_class = ['bench', 'test']
date_class = ['nots', 'ts']
level_class = ['nolvl', 'lvl']
backtrace_class = ['notr', 'tr']

test_files = []
test_name_template = '@0@-@1@-@2@-@3@'
foreach test_type: ['bench', 'test']
	test_src = test_type + '_pinelog.c'
	foreach date_arg: [0, 1]
		date_def = '-DPINELOG_SHOW_DATE=' + date_arg.to_string()
		date_name = date_arg == 1 ? 'ts' : 'nots'

		foreach level_arg: [0, 1]
			level_def = '-DPINELOG_SHOW_LEVEL=' + level_arg.to_string()
			level_name = level_arg == 1 ? 'lvl' : 'nolvl'

			foreach backtrace_arg: [0, 1]
				backtrace_def = '-DPINELOG_SHOW_BACKTRACE=' + backtrace_arg.to_string()
				backtrace_name = backtrace_arg == 1 ? 'tr' : 'notr'

				test_name = test_name_template.format(test_type,
					date_name, level_name, backtrace_name)
				test_exe = executable(test_name, test_src, 'pinelog.c',
					c_args: [
						'-DPINELOG_FATAL_STR="F"',
						'-DPINELOG_ERROR_STR="E"',
						'-DPINELOG_WARNING_STR="W"',
						'-DPINELOG_INFO_STR="I"',
						'-DPINELOG_DEBUG_STR="D"',
						'-DPINELOG_TRACE_STR="T"',
						'-DPINELOG_DEFAULT_LEVEL=PINELOG_LVL_TRACE',
						'-DPINELOG_DEFAULT_STREAM=stderr',
						'-DPINELOG_TEST',
						date_def, level_def, backtrace_def
						])

				test(test_name, test_exe, protocol: 'tap')
			endforeach
		endforeach
	endforeach
endforeach
