cmake_minimum_required(VERSION 3.25)
project(oracles C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

option(ORACLES_SDL "Build the SDL3 frontend" ON)

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
  find_package(SDL3 CONFIG QUIET)
  if(NOT SDL3_FOUND)
    include(FetchContent)
    FetchContent_Declare(SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-3.2.10)
    FetchContent_MakeAvailable(SDL3)
  endif()
  add_executable(oracles src/platform/sdl_main.c src/platform/window.c)
  target_link_libraries(oracles oraclescore SDL3::SDL3)
  add_executable(oracles-native src/platform/app.c src/platform/window.c)
  target_link_libraries(oracles-native oraclesnative oraclesui SDL3::SDL3)
  target_compile_definitions(oracles-native PRIVATE ORACLES_NATIVE=1)
  if(APPLE)
    add_executable(oracles-native-app MACOSX_BUNDLE src/platform/app.c src/platform/window.c)
    target_link_libraries(oracles-native-app oraclesnative oraclesui SDL3::SDL3)
    target_compile_definitions(oracles-native-app PRIVATE ORACLES_NATIVE=1)
    set_target_properties(oracles-native-app PROPERTIES
      OUTPUT_NAME "Oracles"
      MACOSX_BUNDLE_BUNDLE_NAME "Oracles"
      MACOSX_BUNDLE_GUI_IDENTIFIER "dev.oracles-decomp.oracles"
      MACOSX_BUNDLE_BUNDLE_VERSION "0.5"
      MACOSX_BUNDLE_SHORT_VERSION_STRING "0.5")
  endif()
endif()

add_library(oraclesui STATIC src/ui/ui.c src/ui/savefile.c src/ui/launcher.c src/ui/menu.c src/ui/settings.c src/ui/filter.c src/ui/controls.c)
target_include_directories(oraclesui PUBLIC src)
if(NOT WIN32)
  target_link_libraries(oraclesui PUBLIC m)
endif()

enable_testing()
add_executable(test_ui tests/test_ui.c)
target_link_libraries(test_ui oraclesui)
add_test(NAME ui COMMAND test_ui)
set(TEST_ROM_DIR ${CMAKE_SOURCE_DIR}/tests/roms)
set(GAME_ROM_DIR ${CMAKE_SOURCE_DIR}/roms)
add_executable(test_native_tas tests/test_native_tas.c)
target_link_libraries(test_native_tas oraclesnative)
target_compile_definitions(test_native_tas PRIVATE GAME_ROM_DIR="${GAME_ROM_DIR}" TAS_DIR="${CMAKE_SOURCE_DIR}/tas")
add_test(NAME native_tas COMMAND test_native_tas)
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
