cmake_minimum_required(VERSION 3.25)
project(oracles C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

option(ORACLES_SDL "Build the SDL3 frontend" ON)

file(GLOB ORACLES_GAME_SRC ${CMAKE_SOURCE_DIR}/src/game/gen_bank*.c)
add_library(oraclescore STATIC ${ORACLES_GAME_SRC}
  src/core/gb.c
  src/core/bus.c
  src/core/cpu.c
  src/core/timer.c
  src/core/ppu.c
  src/core/apu.c
  src/core/sha1.c
  src/core/hash.c
  src/platform/render.c
  src/platform/png.c
  src/platform/tas.c
  src/platform/setup.c
  src/hooks/hooks.c
)
target_include_directories(oraclescore PUBLIC src)

add_executable(oracles-run src/platform/headless.c)
target_link_libraries(oracles-run oraclescore)

if(ORACLES_SDL)
  find_package(SDL3 CONFIG QUIET)
  if(NOT SDL3_FOUND)
    include(FetchContent)
    FetchContent_Declare(SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-3.2.10)
    FetchContent_MakeAvailable(SDL3)
  endif()
  add_executable(oracles src/platform/sdl_main.c)
  target_link_libraries(oracles oraclescore SDL3::SDL3)
endif()

enable_testing()
set(TEST_ROM_DIR ${CMAKE_SOURCE_DIR}/tests/roms)
set(GAME_ROM_DIR ${CMAKE_SOURCE_DIR}/roms)
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
