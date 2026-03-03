#!/bin/bash
set -euo pipefail

SRC=$(dirname "${BASH_SOURCE[0]}")

outp="./out"
doneoutp=false
HELP="Build &/or package the program in various ways

Optional flags:
  --help - show this help screen
  --built <file> - does not build and instead assumes the program is already built and located at <file>
  -no-pkg - do not create a compressed package or appimage (still puts it in a folder)
  -no-appim - do not package the program into an appimage, only a compressed package
  -no-cmp - do not create a compressed package, only an appimage

  --output <folder> - path to output built stuff to (defaults to $outp)
"
built=""
nopkg="none"
while [[ $# -gt 0 ]]; do
  case "$1" in
    --help | -help | -h) echo "$HELP"; exit 0 ;;

    -no-pkg | -no-appim | -no-cmp)
        if [ "$nopkg" != "none" ]; then
            nopkg="pkg" # Multiple packaging requirements = no packaging
        else
            nopkg="${arg:4}" # Strip the -no-
        fi
    ;;

    --built)
        if [ "$built" != "" ]; then
            echo "Cannot have multiple -built arguments!"
            exit 1
        fi
        built="$2"
        shift
    ;;
    --output)
        if doneoutp; then
            echo "Cannot have multiple --output arguments!"
            exit 1
        fi
        doneoutp=true
        outp="$2"
        shift
    ;;
    *)
        echo "Unknown option '$1'!"
        echo "$HELP"
        exit 1
    ;;
  esac
  shift
done

fprint() {
    printf "\e[1;93m%s\e[0m\n" "$1";
}


if [ "$built" = "" ]; then
    fprint "Building..."
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
    cmake --build build --parallel
    built="build"
else
    fprint "Using built app!"
fi

fprint "Setting up output folder..."
rm -rf "$outp"
mkdir -p "$outp"
cp "$built"/outPlugs/* "$outp"


fprint "Generating appdir..."
rm -rf AppDir

mkdir -p AppDir/usr/bin
cp "$built"/Sapla AppDir/usr/bin/Sapla
cp "$built"/libcore.so AppDir/usr/bin/libcore.so
cp "$built"/libbuiltin.so AppDir/usr/bin/libbuiltin.so

mkdir -p AppDir/usr/lib
ldd "$built"/libcore.so | awk '/=>/ { print $(NF-1) }' | while read lib; do
    if [[ -f "$lib" ]]; then
        cp -u "$lib" AppDir/usr/lib/
    fi
done

cat <<EOF > AppDir/Sapla.desktop
[Desktop Entry]
Type=Application
Name=Sapla
Exec=Sapla
Icon=Sapla
Categories=Utility;
EOF
cp "$SRC"/icon.png AppDir/Sapla.png

cat <<'EOF' > AppDir/AppRun
#!/usr/bin/env bash
exec $APPDIR/usr/bin/Sapla
EOF
chmod +x AppDir/AppRun

if [ "$nopkg" = "pkg" ]; then
    fprint "Not packaging!"
    fprint "Done!"
    exit 0
fi

if [ "$nopkg" != "cmp" ]; then
    fprint "Compressing folder..."
    tar -czf "$outp/Sapla.tar.gz" AppDir/
else
    fprint "Not compressing!"
fi

if [ "$nopkg" != "appim" ]; then
    fprint "Packaging appimage..."
    if ! command -v appimagetool &>/dev/null; then
        found=false
        for pth in \
            "$HOME/Downloads/appimagetool/appimagetool" \
            "/tmp/appimagetool/appimagetool"
        do
            if [ -f "$pth" ]; then
                echo "\`appimagetool\` not found in PATH but found at $pth, using that"
                found=true
                PATH="$PATH:$(dirname "$pth")"
                break
            fi
        done
        if ! $found; then
            echo "\`appimagetool\` not found. Do you want to download it (will not install permanently)?"
            read -s -n 1 -p "[d - download to ~/Downloads, t - download to /tmp, q - quit] " RESP
            echo
            case "$RESP" in
                d) pth="$HOME/Downloads" ;;
                t) pth="/tmp" ;;
                *)
                    echo "Quitting."
                    exit 0
                ;;
            esac
            outpth="$pth/appimagetool"
            fprint "Downloading appimagetool to $outpth..."
            mkdir -p "$outpth"
            wget -P "$outpth" https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
            mv "$outpth/appimagetool-x86_64.AppImage" "$outpth/appimagetool"
            chmod +x "$outpth/appimagetool"
            fprint "Downloaded to $outpth/appimagetool"
            PATH="$PATH:$outpth"
        fi
    fi


    rm -f Sapla*.AppImage
    unset SOURCE_DATE_EPOCH
    appimagetool AppDir
    chmod +x $(ls | grep Sapla*.AppImage)
    mv Sapla*.AppImage "$outp"
else
    fprint "Not building appimage!"
fi

fprint "Done!"
