# mkwallpaper

cli wallpaper generator using pangocairo

This allows generating flat or gradient images in `svg` or
`png` format designed to be used as wallpapers otherwise
know as desktop backgrounds.

It supports adding some text to the image and an optional icon.
You can even add a background image which is useful if you want
to add an icon or text to that, or just use gimp! However this is
very easy to script with a ton of options and if you save to svg
format (default) it is very fast to batch produce a couple of
hundred of images.

## build

```
meson setup build
meson compile -C build
# optionally as root
meson install -C build # installs to /usr/local/bin by default
```

## colors

As *cairo* is used as the backend for this program, which uses the 
*sRGB* color space (see IEC 61966-2-1:1999), all floating-point
arguments should be expressed on the CLI with a *period* (unicode
char U+002E) as the decimal separator.

## bugs

Report any bugs to the _issues_ section here.

Any features, add them yourself and create a PR.


