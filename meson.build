project('cdson', 'c',
        version: '1.0.0',
        default_options: [
            'c_std=c99',
            'warning_level=2',
            'werror=true',
        ])

# Add -lm portably (per meson docs)
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)
deps = [m_dep]

# For asprintf + vasprintf.
if not meson.is_subproject()
    add_global_arguments('-D_GNU_SOURCE', language: 'c')
endif

inc = include_directories('.', 'src')
cdson = library('cdson',
                'src/dump.c', 'src/sniff.c', 'src/fetch.c', 'src/unicode.c',
                include_directories: inc,
                dependencies: deps,
                version: meson.project_version(),
                install: true)
pkg = import('pkgconfig')
pkg.generate(cdson)

install_headers('cdson.h')

cdson_dep = declare_dependency(include_directories: inc, link_with: cdson)

dumpbase = executable('dumpbase', 'tests/dumpbase.c',
                      dependencies: deps,
                      link_with: cdson,
                      install: false)
test('dumpbase', dumpbase)

specparse = executable('specparse', 'tests/specparse.c',
                       dependencies: deps,
                       link_with: cdson,
                       install: false)
test('specparse', specparse)

errors = executable('errors', 'tests/errors.c',
                    dependencies: deps,
                    link_with: cdson,
                    install: false)
test('errors', errors)

escapes = executable('escapes', 'tests/escapes.c',
                     dependencies: deps,
                     link_with: cdson,
                     install: false)
test('escapes', escapes)

fetching = executable('fetching', 'tests/fetching.c',
                      dependencies: deps,
                      link_with: cdson,
                      install: false)
test('fetching', fetching)

# Local variables:
# indent-tabs-mode: nil
# End:
