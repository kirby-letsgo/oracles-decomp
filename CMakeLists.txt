cmake_minimum_required(VERSION 3.25)
project(oracles C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
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
set(ORACLES_SYNC_URL "http://q4ycop2e9hkkq9sxwsg87i79.46.62.205.95.sslip.io" CACHE STRING "Default sync server URL")

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
  add_executable(oracles-native src/platform/app.c src/platform/window.c src/platform/sync_client.c)
  target_link_libraries(oracles-native oraclesnative oraclesui oraclessync oracleswide ${ORACLES_SDL_TARGET})
  target_compile_definitions(oracles-native PRIVATE ORACLES_NATIVE=1 ORACLES_SYNC_URL="${ORACLES_SYNC_URL}")
  if(WIN32)
    enable_language(RC)
    target_sources(oracles-native PRIVATE icon/oracles.rc)
    set_target_properties(oracles-native PROPERTIES WIN32_EXECUTABLE TRUE OUTPUT_NAME Oracles)
  endif()
  if(APPLE)
    set(ORACLES_ICNS ${CMAKE_SOURCE_DIR}/icon/Oracles.icns)
    set_source_files_properties(${ORACLES_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    add_executable(oracles-native-app MACOSX_BUNDLE src/platform/app.c src/platform/window.c src/platform/sync_client.c ${ORACLES_ICNS})
    target_link_libraries(oracles-native-app oraclesnative oraclesui oraclessync oracleswide ${ORACLES_SDL_TARGET})
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

add_library(oraclesui STATIC src/ui/ui.c src/ui/savefile.c src/ui/launcher.c src/ui/menu.c src/ui/settings.c src/ui/filter.c src/ui/controls.c src/ui/touch.c src/ui/syncui.c)
target_include_directories(oraclesui PUBLIC src)
add_library(oracleswide STATIC src/wide/wide.c)
target_include_directories(oracleswide PUBLIC src)
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

# The Android app: SDL3 from source, linked statically into libmain.so with the engine and the UI.
if(ANDROID)
  enable_language(CXX)   # SDL's Android HIDAPI is C++, so libmain links the C++ runtime
  oracles_fetch_sdl()
  add_library(main SHARED src/platform/app.c src/platform/window.c src/platform/sync_client.c)
  target_link_libraries(oraclessync PUBLIC ${ORACLES_SDL_TARGET})
  target_link_libraries(main oraclesnative oraclesui oraclessync oracleswide ${ORACLES_SDL_TARGET} log)
  target_compile_definitions(main PRIVATE ORACLES_NATIVE=1 ORACLES_SYNC_URL="${ORACLES_SYNC_URL}")
  set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)
  return()
endif()

enable_testing()
add_executable(test_ui tests/test_ui.c)
target_link_libraries(test_ui oraclesui)
add_test(NAME ui COMMAND test_ui)
add_executable(test_sync tests/test_sync.c)
target_link_libraries(test_sync oraclessync)
add_test(NAME sync COMMAND test_sync)
add_executable(test_sync_http tests/test_sync_http.c)
target_link_libraries(test_sync_http oraclessync)
add_test(NAME sync_http COMMAND test_sync_http)
set_tests_properties(sync_http PROPERTIES SKIP_RETURN_CODE 77)
set(TEST_ROM_DIR ${CMAKE_SOURCE_DIR}/tests/roms)
set(GAME_ROM_DIR ${CMAKE_SOURCE_DIR}/roms)
add_executable(test_native_tas tests/test_native_tas.c)
target_link_libraries(test_native_tas oraclesnative)
target_compile_definitions(test_native_tas PRIVATE GAME_ROM_DIR="${GAME_ROM_DIR}" TAS_DIR="${CMAKE_SOURCE_DIR}/tas")
add_test(NAME native_tas COMMAND test_native_tas)
add_executable(test_wide tests/test_wide.c)
target_link_libraries(test_wide oracleswide oraclesnative)
target_compile_definitions(test_wide PRIVATE GAME_ROM_DIR="${GAME_ROM_DIR}" TAS_DIR="${CMAKE_SOURCE_DIR}/tas")
add_test(NAME wide COMMAND test_wide)
set_tests_properties(wide PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
set_tests_properties(native_tas PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
set(NATIVE_BINARIES $<TARGET_FILE:oracles-native-run> $<TARGET_FILE:test_native_tas>)
if(ORACLES_SDL)
  list(APPEND NATIVE_BINARIES $<TARGET_FILE:oracles-native>)
  add_test(NAME app_menus COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/test_app_menus.py $<TARGET_FILE:oracles-native>
    "${GAME_ROM_DIR}/Legend of Zelda, The - Oracle of Seasons (USA, Australia).gbc")
  set_tests_properties(app_menus PROPERTIES SKIP_RETURN_CODE 77 LABELS "rom")
endif()
add_test(NAME native_binary COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/check_native_binary.py $<TARGET_FILE:oraclesnative> ${NATIVE_BINARIES})

foreach(t sha1 bus cpu blargg ppu acid2 tas smoke)
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
