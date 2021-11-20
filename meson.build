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

libcdson = library('libcdson', 'cdson.h', 'src/dump.c', 'src/sniff.c',
                   'src/allocation.h', 'src/unicode.h', 'src/unicode.c',
                   dependencies: deps,
                   install: true)

install_headers('cdson.h')

dumpbase = executable('dumpbase', 'tests/dumpbase.c',
                      dependencies: deps,
                      link_with: libcdson,
                      install: false)
test('dumpbase', dumpbase)

specparse = executable('specparse', 'tests/specparse.c',
                       dependencies: deps,
                       link_with: libcdson,
                       install: false)
test('specparse', specparse)

# Local variables:
# indent-tabs-mode: nil
# End:
