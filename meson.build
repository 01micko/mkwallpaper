project(
  'mkwallpaper',
  'c',
  version: '0.1.9',
  license: 'GPL-2.0-only',
  meson_version: '>=0.59.0',
  default_options: [
    'c_std=c11',
    'warning_level=2',
  ],
)

pangocairo = dependency('pangocairo')

executable(
  meson.project_name(),
  sources: files('mkwallpaper.c'),
  dependencies: [pangocairo],
  install: true,
)

install_data('mkwalldemo.sh', install_dir : 'sbin')

subdir('data')
