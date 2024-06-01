scdoc = find_program('scdoc', required: false)

if scdoc.found()
  pages = [ 'mkwallpaper.1', 'mkwalldemo.sh.1' ]
  foreach p : pages
    markdown = p + '.scd'
    manpage = p
    custom_target(
      manpage,
      output: manpage,
      input: markdown,
      command: scdoc,
      feed: true,
      capture: true,
      install: true,
      install_dir : join_paths(get_option('datadir'), 'man', 'man1')
  )
  endforeach
endif

install_data('tux.png', install_dir : get_option('datadir') / 'pixmaps')
