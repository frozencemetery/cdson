project('cdson', 'c',
	default_options: [
	    'c_std=c99',
	    'warning_level=2',
	    'werror=true',
	])

# Add -lm portably (per meson docs)
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)
deps = [m_dep]

library('libcdson', 'cdson.c', 'cdson.h',
	dependencies: deps,
	install: true)

install_headers('cdson.h')

testprog = executable('testprog', 'testprog.c',
		      dependencies: deps,
		      install: false)

test('testprog', testprog)
