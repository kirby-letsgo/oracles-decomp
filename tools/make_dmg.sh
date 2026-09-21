#!/bin/sh
# Build the macOS app bundle from the SDL tree and wrap it in a DMG.
# usage: tools/make_dmg.sh [BUILD_DIR] [OUT.dmg]
set -e
build=${1:-build-sdl}
out=${2:-$build/Oracles.dmg}
cmake --build "$build" --target oracles-native-app
app="$build/Oracles.app"
test -d "$app" || { echo "no bundle at $app" >&2; exit 1; }
# SDL3 comes from Homebrew or the FetchContent build; carry it inside the bundle and sign ad hoc.
exe="$app/Contents/MacOS/Oracles"
sdl=$(otool -L "$exe" | awk '/libSDL3/ {print $1}')
if [ -n "$sdl" ] && [ "${sdl#@}" = "$sdl" ]; then
  mkdir -p "$app/Contents/Frameworks"
  cp "$sdl" "$app/Contents/Frameworks/"
  install_name_tool -change "$sdl" "@executable_path/../Frameworks/$(basename "$sdl")" "$exe"
  codesign --force --sign - "$app/Contents/Frameworks/$(basename "$sdl")" "$exe"
fi
rm -f "$out"
hdiutil create -volname Oracles -srcfolder "$app" -ov -format UDZO "$out" >/dev/null
echo "$out"
