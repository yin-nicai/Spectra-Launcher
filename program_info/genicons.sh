#!/bin/bash

LAUNCHER_APPID="org.spectra.Spectra"

svg2png() {
    input_file="$1"
    output_file="$2"
    width="$3"
    height="$4"

    inkscape -w "$width" -h "$height" -o "$output_file" "$input_file"
}

if command -v "inkscape" && command -v "icotool" && command -v "oxipng"; then
    # Windows ICO
    d=$(mktemp -d)

    svg2png ${LAUNCHER_APPID}.svg "$d/spectra_16.png" 16 16
    svg2png ${LAUNCHER_APPID}.svg "$d/spectra_24.png" 24 24
    svg2png ${LAUNCHER_APPID}.svg "$d/spectra_32.png" 32 32
    svg2png ${LAUNCHER_APPID}.svg "$d/spectra_48.png" 48 48
    svg2png ${LAUNCHER_APPID}.svg "$d/spectra_64.png" 64 64
    svg2png ${LAUNCHER_APPID}.svg "$d/spectra_128.png" 128 128
    svg2png ${LAUNCHER_APPID}.svg "$d/spectra_256.png" 256 256

    oxipng --opt max --strip all --alpha --interlace 0 "$d/spectra_"*".png"

    rm spectra.ico && icotool -o spectra.ico -c \
        "$d/spectra_256.png"  \
        "$d/spectra_128.png"  \
        "$d/spectra_64.png"   \
        "$d/spectra_48.png"   \
        "$d/spectra_32.png"   \
        "$d/spectra_24.png"   \
        "$d/spectra_16.png"
else
    echo "ERROR: Windows icons were NOT generated!" >&2
    echo "ERROR: requires inkscape, icotool and oxipng in PATH"
fi

if command -v "inkscape" && command -v "iconutil" && command -v "oxipng"; then
    # macOS ICNS
    d=$(mktemp -d)

    d="$d/spectra.iconset"

    mkdir -p "$d"

    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_16x16.png" 16 16
    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_16x16@2x.png" 32 32
    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_32x32.png" 32 32
    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_32x32@2x.png" 64 64
    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_128x128.png" 128 128
    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_128x128@2x.png" 256 256
    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_256x256.png" 256 256
    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_256x256@2x.png" 512 512
    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_512x512.png" 512 512
    svg2png ${LAUNCHER_APPID}.bigsur.svg "$d/icon_512x512@2x.png" 1024 1024

    oxipng --opt max --strip all --alpha --interlace 0 "$d/icon_"*".png"

    iconutil -c icns "$d"
    cp -v "$d/spectra.icns" .
else
    echo "ERROR: macOS icons were NOT generated!" >&2
    echo "ERROR: requires inkscape, iconutil and oxipng in PATH"
fi

# replace icon in themes
cp -v ${LAUNCHER_APPID}.svg "../launcher/resources/multimc/scalable/launcher.svg"
