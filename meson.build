project('shika','c')

glib_deps = dependency('glib-2.0')
gio_deps = dependency('gio-2.0')
libsoup_deps = dependency('libsoup-2.4')

shika_src = [
    'shika.h',
    'shika.c',
    'shikacontent.h',
    'shikacontent.c',
    'shikaapplication.h',
    'shikaapplication.c',
    'shikainstance.h',
    'shikainstance.c'
]

shika_bin = executable('shika',shika_src,dependencies: [glib_deps, gio_deps, libsoup_deps ],install: true)
test('shika',shika_bin)
