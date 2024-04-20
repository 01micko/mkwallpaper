# Demo Dir

This directory contains wallpapers and the command to reproduce them.

They may require fonts you dont have. 

## Reproducer

### LinuxTux

Requires `labwcmoon.ttf`, Nunito fonts and a png of Tux.

```
mkwallpaper -z'.1 .4 .5' -x 1920 -y 1080 -k -c -fNunito -l "$(/usr/bin/printf "%s  %b  %s" "I" "\\ue9dc" "Linux")" -e /home/mick/Tux120x142.png -nLinuxTux -s60
```
