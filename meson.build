project('rpi-fan-ctrl', 'c', version: '0.1', default_options: ['warning_level=3'])

libpigpio = meson.get_compiler('c').find_library('pigpio', has_headers: ['pigpio.h'], required: false)
threads = dependency('threads')

include_dirs = []
if not libpigpio.found()
  include_dirs += include_directories('pigpio-headers')
endif
include_dirs += include_directories('src')

src_files = []
src_files += files('src/client.c')
src_files += files('src/server.c')
src_files += files('src/unix_socket.c')
src_files += files('src/utils.c')

executable(
    meson.project_name(),

    src_files,

    'main.c',

    dependencies: [libpigpio, threads],

    include_directories : include_dirs,

    install: true
)
