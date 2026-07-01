cmake_minimum_required(VERSION 3.31)
project(MiisendU-Wii)

find_library(FAT fat
  PATHS "${OGC_ROOT}/lib/${OGC_SUBDIR}"
  REQUIRED
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(PNG REQUIRED libpng)
pkg_check_modules(FREETYPE REQUIRED freetype2)

include(FetchContent)
FetchContent_Declare(inipp
  URL https://github.com/mcmtroffaes/inipp/archive/3f224f1eed7a67d5d7e5fc8cab72de02a056b966.tar.gz
)
FetchContent_Declare(rapidjson
  URL https://github.com/Tencent/rapidjson/archive/refs/heads/master.tar.gz
)
FetchContent_Declare(grrlib
  URL https://github.com/GRRLIB/GRRLIB/archive/refs/heads/master.tar.gz
)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "Disable rapidjson documentation build" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "Disable rapidjson examples build" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "Disable rapidjson tests build" FORCE)
set(RAPIDJSON_BUILD_CXX11 OFF CACHE BOOL "Disable C++11" FORCE)
set(RAPIDJSON_BUILD_CXX17 OFF CACHE BOOL "Disable C++17" FORCE)
set(RAPIDJSON_BUILD_CXX20 ON CACHE BOOL "Enable C++20" FORCE)
FetchContent_MakeAvailable(inipp rapidjson grrlib)

add_executable(MiisendU-Wii)

target_compile_features(MiisendU-Wii PRIVATE cxx_std_26)

target_compile_options(MiisendU-Wii PRIVATE
  -Werror
  -Wall
  -Wextra
  -Wshadow
  -Wnon-virtual-dtor
  -Wold-style-cast
  -Wcast-align
  -Wunused
  -Woverloaded-virtual
  -Wpedantic
  -Wnull-dereference
  -Wdouble-promotion
  -Wimplicit-fallthrough
  -Wmisleading-indentation
  -Wduplicated-cond
  -Wduplicated-branches
  -Wsuggest-override
)

target_sources(MiisendU-Wii PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/application.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/udp.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/pad_to_json.cpp"
)

target_include_directories(MiisendU-Wii PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/source"
  "${rapidjson_SOURCE_DIR}/include"
)

include(cmake/embedded-tpl.cmake)
add_embedded_tpl(data
  "${CMAKE_CURRENT_SOURCE_DIR}/data/textures.scf"
)

target_link_libraries(MiisendU-Wii PRIVATE
  data
  grrlib
  ${FAT}
  ${FREETYPE_LIBRARIES}
  inipp::inipp
)

ogc_create_dol(MiisendU-Wii)
