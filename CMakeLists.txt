cmake_minimum_required(VERSION 3.18)
project(MiisendU VERSION 0.0.1)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/source/version.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/version.h"
)

find_library(FAT fat
  PATHS "${OGC_ROOT}/lib/${OGC_SUBDIR}"
  REQUIRED
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(PNG REQUIRED libpng)
pkg_check_modules(FREETYPE REQUIRED freetype2)

include(FetchContent)
FetchContent_Declare(inipp
  URL https://github.com/mcmtroffaes/inipp/archive/3c1668812026f1a94471b85ac5ab11ab87c43607.tar.gz
)
FetchContent_Declare(rapidjson
  URL https://github.com/Tencent/rapidjson/archive/refs/heads/master.tar.gz
)
FetchContent_Declare(grrlib
  URL https://github.com/GRRLIB/GRRLIB/archive/refs/heads/master.tar.gz
)
FetchContent_Populate(rapidjson)
FetchContent_MakeAvailable(inipp grrlib)

add_executable(MiisendU)

target_compile_features(MiisendU PRIVATE cxx_std_20)

target_compile_options(MiisendU PRIVATE
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

target_sources(MiisendU PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/application.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/udp.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/source/pad_to_json.cpp"
)
if(CMAKE_SYSTEM_NAME STREQUAL "NintendoGameCube")
  target_sources(MiisendU PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/source/application_gc.cpp"
  )
  target_link_libraries(MiisendU PRIVATE
    bba
  )
  set_target_properties(MiisendU PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-GC")
else()
  target_sources(MiisendU PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/source/application_wii.cpp"
  )
  set_target_properties(MiisendU PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-Wii")
endif()

target_include_directories(MiisendU PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/source"
  "${rapidjson_SOURCE_DIR}/include"
)

include(cmake/embedded-tpl.cmake)
add_embedded_tpl(data
  "${CMAKE_CURRENT_SOURCE_DIR}/data/textures.scf"
)

target_link_libraries(MiisendU PRIVATE
  data
  grrlib
  ${FAT}
  ${FREETYPE_LIBRARIES}
  inipp::inipp
)

ogc_create_dol(MiisendU)
