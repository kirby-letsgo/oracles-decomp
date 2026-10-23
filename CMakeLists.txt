cmake_minimum_required(VERSION 3.25)
project(oracles C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# C++ is only for the vendored xBRZ scaler in third_party/xbrz; everything else is C.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MINGW)
  # Linking any C++ makes the binaries want libstdc++-6.dll and libgcc_s_seh-1.dll beside them, and
  # the Windows build is meant to be one file that needs only system DLLs.
  add_link_options(-static-libstdc++ -static-libgcc -static)
endif()
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

if(ANDROID)
  option(ORACLES_SDL "Build the SDL3 frontend" OFF)
else()
  option(ORACLES_SDL "Build the SDL3 frontend" ON)
endif()
# Release builds (and Android) compile SDL from source and link it statically, so each app is one
# binary; otherwise an installed SDL3 is used when there is one.
option(ORACLES_SDL_VENDORED "Build SDL3 from source and link it statically" OFF)
# The save-sync server the app uses until the player sets another in sync.ini
set(ORACLES_SYNC_URL "https://oracles.decomps-sync.top" CACHE STRING "Default sync server URL")

macro(oracles_fetch_sdl)
  include(FetchContent)
  set(SDL_SHARED OFF CACHE BOOL "" FORCE)
  set(SDL_STATIC ON CACHE BOOL "" FORCE)
  set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.16/SDL3-3.4.16.tar.gz
    URL_HASH SHA256=7322236cd12090c3eb40b9728be4d49c76f66ad17d04369584d4ecad5cf77c68)
  FetchContent_MakeAvailable(SDL3)
  set(ORACLES_SDL_TARGET SDL3::SDL3-static)
endmacro()

file(GLOB_RECURSE ORACLES_GAME_SRC CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/game/*.c")
set(ORACLES_SHARED_SRC ${ORACLES_GAME_SRC}
  src/core/gb.c
  src/core/bus.c
  src/core/cpu_common.c
  src/hw/timer.c
  src/hw/ppu.c
  src/hw/apu.c
  src/hw/dma.c
  src/hw/io.c
  src/core/sha1.c
  src/core/hash.c
  src/hw/render.c
  src/platform/png.c
  src/platform/archive.c
  src/platform/tas.c
  src/platform/setup.c
  src/hooks/hooks.c
  src/rt/fibers.c
  src/assets/assets.c
  src/assets/codemap.c
)
add_library(oraclescore STATIC ${ORACLES_SHARED_SRC} src/core/cpu.c)
target_include_directories(oraclescore PUBLIC src)
add_library(oraclesnative STATIC ${ORACLES_SHARED_SRC} src/rt/native.c)
target_include_directories(oraclesnative PUBLIC src)

add_executable(oracles-run src/platform/headless.c)
target_link_libraries(oracles-run oraclescore)
add_executable(oracles-native-run src/platform/headless.c)
target_link_libraries(oracles-native-run oraclesnative)
target_compile_definitions(oracles-native-run PRIVATE ORACLES_NATIVE=1)
add_custom_target(oracles-ref DEPENDS oracles-run)
add_executable(lockstep-native tools/lockstep_native.c tools/lockstep_native_engine.c)
target_link_libraries(lockstep-native oraclescore)

if(ORACLES_SDL)
  if(NOT ORACLES_SDL_VENDORED)
    find_package(SDL3 CONFIG QUIET)
  endif()
  if(SDL3_FOUND)
    set(ORACLES_SDL_TARGET SDL3::SDL3)
  else()
    oracles_fetch_sdl()
  endif()
  add_executable(oracles src/platform/sdl_main.c src/platform/window.c)
  target_link_libraries(oracles oraclescore ${ORACLES_SDL_TARGET})
  add_executable(oracles-native src/platform/app.c src/platform/window.c src/platform/sync_client.c src/platform/haptic.c)
  # oraclesui is not named here: oraclesmods and oraclesgui link it PUBLIC, so naming it again makes
  # the linker warn about a duplicate library.
  target_link_libraries(oracles-native oraclesnative oraclesgui oraclessync oracleswide oraclesmods ${ORACLES_SDL_TARGET})
  target_compile_definitions(oracles-native PRIVATE ORACLES_NATIVE=1 ORACLES_SYNC_URL="${ORACLES_SYNC_URL}")
  if(WIN32)
    enable_language(RC)
    target_sources(oracles-native PRIVATE icon/oracles.rc)
    set_target_properties(oracles-native PROPERTIES WIN32_EXECUTABLE TRUE OUTPUT_NAME Oracles)
  endif()
  if(APPLE)
    set(ORACLES_ICNS ${CMAKE_SOURCE_DIR}/icon/Oracles.icns)
    set_source_files_properties(${ORACLES_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    add_executable(oracles-native-app MACOSX_BUNDLE src/platform/app.c src/platform/window.c src/platform/sync_client.c src/platform/haptic.c ${ORACLES_ICNS})
    target_link_libraries(oracles-native-app oraclesnative oraclesgui oraclessync oracleswide oraclesmods ${ORACLES_SDL_TARGET})
    target_compile_definitions(oracles-native-app PRIVATE ORACLES_NATIVE=1 ORACLES_SYNC_URL="${ORACLES_SYNC_URL}")
    set_target_properties(oracles-native-app PROPERTIES
      OUTPUT_NAME "Oracles"
      MACOSX_BUNDLE_BUNDLE_NAME "Oracles"
      MACOSX_BUNDLE_ICON_FILE "Oracles.icns"
      MACOSX_BUNDLE_GUI_IDENTIFIER "dev.oracles-decomp.oracles"
      MACOSX_BUNDLE_BUNDLE_VERSION "0.5"
      MACOSX_BUNDLE_SHORT_VERSION_STRING "0.5")
  endif()
endif()

add_library(oraclesui STATIC src/ui/ui.c src/ui/font_builtin.c src/ui/savefile.c src/ui/menu.c src/ui/settings.c src/ui/filter.c src/ui/controls.c src/ui/touch.c src/ui/syncui.c src/ui/home.c src/ui/labels.c
  src/ui/upscale.cpp third_party/xbrz/xbrz.cpp third_party/hqx/hq2x.c third_party/hqx/hq3x.c)
target_include_directories(oraclesui PUBLIC src PRIVATE third_party/xbrz third_party/hqx)
# oraclesui carries C++ objects, so anything linking it links with the C++ driver.
set_target_properties(oraclesui PROPERTIES LINKER_LANGUAGE CXX)
# item.c on its own: oraclesmods needs the icon decoder but must not link the engine, and wide.c
# pulls in framebuffer_to_rgb (and so the engine). Do not merge this back into oracleswide.
add_library(oraclesitem STATIC src/wide/item.c)
target_include_directories(oraclesitem PUBLIC src)
# hud.c calls wide_window_left, so it pulls in the engine too; split it out like oraclesitem if mods
# or ui ever need hud_glyph.
add_library(oracleswide STATIC src/wide/wide.c src/wide/room.c src/wide/hud.c src/wide/npcs.c)
target_include_directories(oracleswide PUBLIC src)
target_link_libraries(oracleswide PUBLIC oraclesitem)
add_library(oraclessync STATIC src/sync/sha256.c src/sync/sync.c src/sync/http_common.c)
target_include_directories(oraclessync PUBLIC src)
if(WIN32)
  target_sources(oraclessync PRIVATE src/sync/http_winhttp.c)
  target_link_libraries(oraclessync PUBLIC winhttp)
elseif(ANDROID)
  target_sources(oraclessync PRIVATE src/sync/http_android.c)
else()
  target_sources(oraclessync PRIVATE src/sync/http_curl.c)
  target_link_libraries(oraclessync PUBLIC ${CMAKE_DL_LIBS})
endif()
if(NOT WIN32)
  target_link_libraries(oraclesui PUBLIC m)
endif()

# The menus at screen resolution (src/gui): Clay lays them out, SDL draws them, in VT323 (OFL),
# which is embedded in the binary. Only the app links it; the engine libraries and headless
# runners never see SDL or the font.
if(ORACLES_SDL OR ANDROID)
  set(ORACLES_FONT ${CMAKE_SOURCE_DIR}/third_party/fonts/VT323-Regular.ttf)
  add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/gui_vt323.c
    COMMAND ${CMAKE_COMMAND} -DIN=${ORACLES_FONT} -DOUT=${CMAKE_BINARY_DIR}/gui_vt323.c -DNAME=gui_vt323 -P ${CMAKE_SOURCE_DIR}/cmake/embed.cmake
    DEPENDS ${ORACLES_FONT} ${CMAKE_SOURCE_DIR}/cmake/embed.cmake)
  add_library(oraclesgui STATIC src/gui/gui.c src/gui/widgets.c src/gui/pause.c src/gui/slots.c src/gui/settings.c
    src/gui/home.c src/gui/controls.c src/gui/sync.c src/gui/pad.c src/gui/vendor.c ${CMAKE_BINARY_DIR}/gui_vt323.c)
  target_include_directories(oraclesgui PUBLIC src third_party/clay PRIVATE third_party/stb)
  set_source_files_properties(src/gui/vendor.c PROPERTIES COMPILE_OPTIONS "-w")
  # Clay's macros build its structs from partial initializers on purpose
  target_compile_options(oraclesgui PRIVATE -Wno-missing-field-initializers)
  target_link_libraries(oraclesgui PUBLIC oraclesui)
  if(ORACLES_SDL)
    target_link_libraries(oraclesgui PUBLIC ${ORACLES_SDL_TARGET})
  endif()
endif()

add_library(oraclesmods STATIC src/mods/overlay.c src/mods/mods.c src/mods/api.c src/mods/sandbox.c
  third_party/lua/lapi.c third_party/lua/lauxlib.c third_party/lua/lbaselib.c
  third_party/lua/lcode.c third_party/lua/lcorolib.c third_party/lua/lctype.c third_party/lua/ldblib.c
  third_party/lua/ldebug.c third_party/lua/ldo.c third_party/lua/ldump.c third_party/lua/lfunc.c
  third_party/lua/lgc.c third_party/lua/linit.c third_party/lua/liolib.c third_party/lua/llex.c
  third_party/lua/lmathlib.c third_party/lua/lmem.c third_party/lua/loadlib.c third_party/lua/lobject.c
  third_party/lua/lopcodes.c third_party/lua/loslib.c third_party/lua/lparser.c third_party/lua/lstate.c
  third_party/lua/lstring.c third_party/lua/lstrlib.c third_party/lua/ltable.c third_party/lua/ltablib.c
  third_party/lua/ltm.c third_party/lua/lundump.c third_party/lua/lutf8lib.c third_party/lua/lvm.c
  third_party/lua/lzio.c)
target_include_directories(oraclesmods PUBLIC src PRIVATE third_party/lua)
target_link_libraries(oraclesmods PUBLIC oraclesui oraclesitem)
if(NOT WIN32)
  target_compile_definitions(oraclesmods PRIVATE LUA_USE_POSIX)
endif()

# The Android app: SDL3 from source, linked statically into libmain.so with the engine and the UI.
if(ANDROID)
  enable_language(CXX)   # SDL's Android HIDAPI is C++, so libmain links the C++ runtime
  oracles_fetch_sdl()
  add_library(main SHARED src/platform/app.c src/platform/window.c src/platform/sync_client.c src/platform/haptic.c)
  target_link_libraries(oraclessync PUBLIC ${ORACLES_SDL_TARGET})
  target_link_libraries(oraclesgui PUBLIC ${ORACLES_SDL_TARGET})
  target_link_libraries(main oraclesnative oraclesgui oraclessync oracleswide oraclesmods ${ORACLES_SDL_TARGET} log)
  target_compile_definitions(main PRIVATE ORACLES_NATIVE=1 ORACLES_SYNC_URL="${ORACLES_SYNC_URL}")
  set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)
  return()
endif()

enable_testing()
set(TEST_ROM_DIR ${CMAKE_SOURCE_DIR}/tests/roms)
set(GAME_ROM_DIR ${CMAKE_SOURCE_DIR}/roms)
add_executable(test_ui tests/test_ui.c)
target_link_libraries(test_ui oraclesui)
add_test(NAME ui COMMAND test_ui)
add_executable(test_mods tests/test_mods.c)
target_link_libraries(test_mods oraclesmods)
target_include_directories(test_mods PRIVATE third_party/lua)
# For the_docs_document_exactly_the_live_api, which reads MODDING.md from the repository root and
# compares it against the live `oracles` table -- the docs-drift gate. A compiled-in absolute path
# rather than a relative one, so the test does not depend on which directory ctest runs it from.
target_compile_definitions(test_mods PRIVATE REPO_ROOT="${CMAKE_SOURCE_DIR}")
add_test(NAME mods COMMAND test_mods)
# TIMEOUT is a backstop, not the primary defence: test_mods's own hang-risk tests each wrap their
# mods_init call in its own alarm() so a real hang fails fast with a named, diagnostic message
# rather than ctest's bare "Timeout" (naming no test) after its 1500s default -- but that
# alarm() only protects the one process calling it, not the next test if something upstream of
# any alarm() call blocks before it's even set. As of this writing there are 21 such alarm()
# sites, 5-20s each, summing to 305s if every single one somehow fired in sequence (which would
# mean nearly everything in the file hung at once -- astronomically unlikely, but the number this
# TIMEOUT must stay above regardless). 400s leaves headroom above that worst case for the rest of
# the suite's own (sub-second-to-low-single-digit-second) running time and ordinary scheduling
# overhead between tests, while still catching, in well under ctest's 1500s default, a hang that
# happens to land before any alarm() call is reached at all.
#
# Deliberately NOT labelled "rom": this file carries no ROM-dependent test any more (the two that
# needed it moved to test_mods_rom, below) precisely so `ctest -LE rom` -- the fast gate, and what
# CI runs -- actually exercises this whole suite instead of skipping it outright. SKIP_RETURN_CODE
# 77 stays, because several tests here still skip for unrelated environment reasons (no root, no
# mkfifo, an unwritable path), via the same RUN_SKIPPABLE mechanism the ROM tests used to share.
set_tests_properties(mods PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 400)
# The two mod tests that need the real ROM, split out so `mods` above can stay unlabelled (see its
# own comment). Kept to the same SKIP_RETURN_CODE/LABELS convention as test_wide/test_room/etc.
add_executable(test_mods_rom tests/test_mods_rom.c)
target_link_libraries(test_mods_rom oraclesmods)
target_compile_definitions(test_mods_rom PRIVATE GAME_ROM_DIR="${GAME_ROM_DIR}")
add_test(NAME mods_rom COMMAND test_mods_rom)
set_tests_properties(mods_rom PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom" TIMEOUT 120)
add_executable(test_sync tests/test_sync.c)
target_link_libraries(test_sync oraclessync)
add_test(NAME sync COMMAND test_sync)
add_executable(test_sync_http tests/test_sync_http.c)
target_link_libraries(test_sync_http oraclessync)
add_test(NAME sync_http COMMAND test_sync_http)
set_tests_properties(sync_http PROPERTIES SKIP_RETURN_CODE 77)
add_executable(test_native_tas tests/test_native_tas.c)
target_link_libraries(test_native_tas oraclesnative)
target_compile_definitions(test_native_tas PRIVATE GAME_ROM_DIR="${GAME_ROM_DIR}" TAS_DIR="${CMAKE_SOURCE_DIR}/tas")
add_test(NAME native_tas COMMAND test_native_tas)
add_executable(test_wide tests/test_wide.c)
target_link_libraries(test_wide oracleswide oraclesnative)
target_compile_definitions(test_wide PRIVATE GAME_ROM_DIR="${GAME_ROM_DIR}" TAS_DIR="${CMAKE_SOURCE_DIR}/tas")
add_test(NAME wide COMMAND test_wide)
add_executable(test_hud tests/test_hud.c)
target_link_libraries(test_hud oracleswide oraclesnative)
add_test(NAME hud COMMAND test_hud)
add_executable(test_rooms tests/test_rooms.c)
target_link_libraries(test_rooms oraclesnative)
target_compile_definitions(test_rooms PRIVATE GAME_ROM_DIR="${GAME_ROM_DIR}" TAS_DIR="${CMAKE_SOURCE_DIR}/tas")
add_test(NAME rooms COMMAND test_rooms)

add_executable(test_room tests/test_room.c)
target_link_libraries(test_room oracleswide oraclesnative)
target_compile_definitions(test_room PRIVATE GAME_ROM_DIR="${GAME_ROM_DIR}" TAS_DIR="${CMAKE_SOURCE_DIR}/tas")
add_test(NAME room COMMAND test_room)
add_executable(test_eu tests/test_eu.c)
target_link_libraries(test_eu oracleswide oraclesnative)
target_compile_definitions(test_eu PRIVATE GAME_ROM_DIR="${GAME_ROM_DIR}" TAS_DIR="${CMAKE_SOURCE_DIR}/tas")
add_test(NAME eu COMMAND test_eu)
set_tests_properties(eu PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
set_tests_properties(rooms PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
set_tests_properties(room PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
set_tests_properties(wide PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
set_tests_properties(native_tas PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
set(NATIVE_BINARIES $<TARGET_FILE:oracles-native-run> $<TARGET_FILE:test_native_tas>)
if(ORACLES_SDL)
  list(APPEND NATIVE_BINARIES $<TARGET_FILE:oracles-native>)
  add_test(NAME app_menus COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/test_app_menus.py $<TARGET_FILE:oracles-native>
    "${GAME_ROM_DIR}/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc")
  set_tests_properties(app_menus PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
  add_test(NAME app_eu COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/test_app_eu.py $<TARGET_FILE:oracles-native>
    "${GAME_ROM_DIR}/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc"
    "${GAME_ROM_DIR}/Legend of Zelda, The - Oracle of Seasons (Europe) (En,Fr,De,Es,It).gbc")
  set_tests_properties(app_eu PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
  add_test(NAME sync_flow COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/test_sync_flow.py $<TARGET_FILE:oracles-native>
    "${GAME_ROM_DIR}/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc")
  set_tests_properties(sync_flow PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
  # Pins app.c's own rd_room (not linked into test_mods, and never exercised by the TAS replays,
  # which call through the engine directly rather than through app.c): installs a mod whose
  # on_room_change logs the room, drives the real app through the start of the known-good Seasons
  # movie, and asserts the handler actually fired with a room other than the pre-game default.
  # The same mod also calls oracles.room() and checks its group/width/height against the backing
  # bytes read independently via oracles.rd -- a useful regression check (e.g. for width/height),
  # but NOT what catches app.c's rd_room_group ternary being swapped: room group is 0 for this
  # whole early-game frame range, and so (confirmed by a WRAM dump) is the address the swapped
  # ternary would read instead, so the two are indistinguishable here. A documented, open gap --
  # see test_mods_room.py's own docstring and this task's report for what was tried (driving the
  # same movie's TAS replay tens of thousands of frames further, by two different input encodings,
  # looking for a non-zero group) and why it did not close this within a practical test budget.
  add_test(NAME mods_room COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/test_mods_room.py $<TARGET_FILE:oracles-native>
    "${GAME_ROM_DIR}/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc" "${CMAKE_SOURCE_DIR}/tas/seasons-play.inputs")
  set_tests_properties(mods_room PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom" TIMEOUT 240)
endif()
add_test(NAME native_binary COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/check_native_binary.py $<TARGET_FILE:oraclesnative> ${NATIVE_BINARIES})

foreach(t sha1 bus cpu blargg ppu acid2 tas smoke archive vwf)
  if(EXISTS ${CMAKE_SOURCE_DIR}/tests/test_${t}.c)
    add_executable(test_${t} tests/test_${t}.c)
    target_link_libraries(test_${t} oraclescore)
    target_compile_definitions(test_${t} PRIVATE
      TEST_ROM_DIR="${TEST_ROM_DIR}" GAME_ROM_DIR="${GAME_ROM_DIR}"
      TAS_DIR="${CMAKE_SOURCE_DIR}/tas")
    add_test(NAME ${t} COMMAND test_${t})
    set_tests_properties(${t} PROPERTIES SKIP_RETURN_CODE 77)
    if(t STREQUAL "tas")
      set_tests_properties(${t} PROPERTIES LABELS "rom")
    endif()
  endif()
endforeach()
