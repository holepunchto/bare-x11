cmake_minimum_required(VERSION 4.0)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)

find_package(PkgConfig REQUIRED)

project(bare_x11 C)

pkg_check_modules(X11 REQUIRED IMPORTED_TARGET GLOBAL x11)

add_bare_module(bare_x11)

target_sources(
  ${bare_x11}
  PRIVATE
    binding.c
)

target_link_libraries(
  ${bare_x11}
  PUBLIC
    PkgConfig::X11
)
