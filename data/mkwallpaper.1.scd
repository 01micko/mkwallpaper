mkwallpaper(1)

# NAME

*mkwallpaper* - Generate SVG or PNG Linux wallpapers.

# SYNOPSIS

*mkwallpaper* [options...]

# DESCRIPTION

Mkwallpaper is a command line utility to generate wallpapers or desktop
backgrounds for unix like systems supporting text labels, icons, other
background images, icon fonts, linear or radial gradients.

It uses Pango for font rendering and manipulation and Cairo for
patterns, icons and other backgrounds. It only uses short options, and
there are many of them.

It is great for shell scripting where you can batch produce many images
or even overlay previously produced images in a loop if saving to
.png files (*-p* option).

# OPTIONS

*-h*

	show help and exit

*-v*

	show version and exit

*-n*

	image file name [string]

*-l*

	label for an image, up to 36 chars, quoted (optional) [string]

*-f*

	a TTF font family; quoted if it has spaces [string]

*-z*

	floating point RGB, quoted, space delimited values for colour ["double"] 

*-p*

	save file as "png"; "svg" format is the default [no arg]

*-x*

	width of image in pixels [int]

*-y*

	height of image in pixels [int]

*-s*

	font size in pixels [int]

*-k*

	add embossed effect on font [no arg]

*-g*

	turn on background linear gradient (default is off) [no arg]

*-r*

	turn on background radial gradient (default is off) [no arg]

*-q*

	radial gradient position in px "x y"; requires "-r" ["int int"]

*-e*

	'/path/to/icon.png x y' - embed a png image at position "x y"(optional)

*-w*

	'/path/to/background.png' - embed a png wall at pos 0,0 (optional)
	Requires original dimensions of incoming png image for
	options '-x' and '-y'; (overrides "-z")

*-c*

	centre font and/or icon on background image [no arg]

*-j*

	font position in px "x y"; overrides "-c" ["int int"]

*-o*

	gradient offset: floating point value from 0.0 to 1.0 [double]

*-a*

	angle: integer value from 0 to 20 for the gradient angle [int]

*-d*

	destination directory: (default: $HOME) [/path/to/directory] 

*-b*

	font-style: accepted values are "n" (normal) [the default], "b" (bold),
	"i" (italic), "o" (bold-italic)

*-u*

	use transparent background (overrides "-z"; not good for wallpapers)

# COLORS

	As *cairo* is used as the backend for this program, which uses the 
	*sRGB* color space (see IEC 61966-2-1:1999), all floating-point
	arguments should be expressed on the CLI with a *period* (unicode
	char U+002E) as the decimal separator.

# AUTHOR

	Mick Amadio <01micko@gmail.com>

# LICENCE

	GPLv2

# REPORT BUGS

	https://github.com/01micko/mkwallpaper/issues
