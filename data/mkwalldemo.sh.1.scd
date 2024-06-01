mkwalldemo.sh(1)

# NAME

*mkwalldemo.sh* - Demonstration script producing SVG Linux wallpapers.

# SYNOPSIS

*mkwalldemo.sh* [options...]

# DESCRIPTION

This is a demonstration script to show off the capabilities of
*mkwallpaper*. It generates 10 images in SVG format in your HOME
directory sourcing your */etc/os-release* or */usr/lib/os-release* file
and attempts to download an icon or optionally use one that already
resides on your system. Only a handfull of distros are supported for the
downloading of the icon. It is a demonstration after all.

# OPTIONS

*-h|--help*

	show help and exit

*-v|--version*

	show version and exit

*-i*

	/absolute/path/to/icon_file.png
	This overrides downloading of a png file from the internet.

# AUTHOR

	Mick Amadio <01micko@gmail.com>

# LICENCE

	GPLv2

# REPORT BUGS

	https://github.com/01micko/mkwallpaper/issues

# SEE ALSO

	mkwallpaper(1)	
