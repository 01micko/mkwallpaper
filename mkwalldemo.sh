#!/usr/bin/env bash

#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#

# Copyright 2024 Mick Amadio <01micko@gmail.com>

# this script tries to download an icon and build wallpapers based on 
# 'os-release' It uses the default sans font and saves to default .svg

# usage
_usage() {
    cat <<EOF
    -h|--help  show help and exit
    -v|--version  show version and exit
    -i /path/to/icon.png - must be a valid png at icon size
EOF
    exit 0
}

# exit on error
exit_error() {
    echo "ERROR: $1"
    exit 1
}

# linear gradient
_linear() {
    mkwallpaper -l $1 -n ${1}-l${2} -z "$3 $4 $5" -k -c -e $6 -x 1920 -y 1080 \
        -s 80 -d $7 -g -a $8 -o $9 && return 0 || return 1
}

# radial gradient
_radial() {
    mkwallpaper -l $1 -n ${1}-r${2} -z "$3 $4 $5" -k -c -e $6 -x 1920 -y 1080 \
        -s 80 -d $7 -r -q "960 480" && return 0 || return 1
}

# download an icon
# I'm amazed that there are no actual databases of stock linux distro
# icons! I've tried to source these responsibly but I could only come
# up with https://librehunt.org/logos/
_grab_ico() {
    wget -t3 https://librehunt.org/logos/${1}.png -P $TEMP \
        && return 0 || return 1
}

export -f exit_error _linear _radial _grab_ico _usage

# main
VER=0.1.0
TEMP=$(mktemp -d /tmp/mkwallXXXX)
trap "rm -rf $TEMP" EXIT

# get distro name
OSR=''
[[ -r "/etc/os-release" ]] && OSR=/etc/os-release
[[ -z "$OSR" ]] && [[ -r "/usr/lib/os-release" ]] && OSR=/usr/lib/os-release
[[ -z "$OSR" ]] && exit_error "Can not find os-release file"
. "$OSR"
ICO=tux # default

# parameters
while [ $# -gt 0 ]; do
    case $1 in
        -i) shift; path=$1 ;; # icon
        -v|--version) echo $VER && exit 0;;
        -h|--help) _usage ;;
    esac
    shift
done

# only support a few majors, this is a demo after all
case $ID in
    debian)NAME=debian;ICO=$NAME;;
    slackware*)NAME=slackware;ICO=$NAME;;
    ubuntu)NAME=ubuntu;ICO=$NAME;;
    arch)NAME=arch;ICO=$NAME;;
    mint)NAME=mint;ICO=$NAME;;
    fedora)NAME=fedora;ICO=$NAME;;
    void)NAME=void;ICO=$NAME;;
    *)NAME=$ID;ICO=$ICO;;
esac

# get the icon if needed
if [[ -n "$path" ]];then
    ICOPATH=$path
    [[ "${path#*\.}" == 'png' ]] || exit_error "Icon is not PNG"
elif [[ "$ICO" == 'tux' ]]; then
    ICOPATH=/usr/share/pixmaps/${ICO}.png
else
    _grab_ico $ICO || exit_error "Failed to download ${ICO}.png"
    ICOPATH=${TEMP}/${ICO}.png
fi

# create linear
x=0;
while [[ $x -lt 5 ]]; do
    off="0.${RANDOM}"
    ang=$((1 + $(($RANDOM % 20))))
    r="0.${RANDOM}"
    g="0.${RANDOM}"
    b="0.${RANDOM}"
    _linear $NAME $x $r $g $b $ICOPATH ${HOME}/${NAME} $ang $off \
        || exit_error "Linear gradient wallpaper creation failed."
    unset off ang r g b
    x=$(($x + 1))
done

# create radial
y=0;
while [[ $y -lt 5 ]]; do
    r="0.${RANDOM}"
    g="0.${RANDOM}"
    b="0.${RANDOM}"
    _radial $NAME $y $r $g $b $ICOPATH ${HOME}/${NAME} \
        || exit_error "Radial gradient wallpaper creation failed."
    unset off ang r g b
    y=$(($y + 1))
done

exit 0
