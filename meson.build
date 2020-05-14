project('rpi-fan-ctrl', 'c', version: '0.1', default_options: ['warning_level=3'])

libpigpio = meson.get_compiler('c').find_library('libpigpio')

executable(
    meson.project_name(),

    'main.c',

    dependencies: [libpigpio],

    install: true
)
