cmake_minimum_required(VERSION 3.18)
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

file(GLOB_RECURSE BIN_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/data/*.png"
)

add_executable(MiisendU-Wii)

target_compile_features(MiisendU-Wii PRIVATE cxx_std_20)

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
  "${CMAKE_CURRENT_SOURCE_DIR}/source/vpad_to_json.cpp"
)

target_include_directories(MiisendU-Wii PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/source"
  "${rapidjson_SOURCE_DIR}/include"
)

dkp_add_embedded_binary_library(data
  ${BIN_FILES}
)

target_link_libraries(MiisendU-Wii PRIVATE
  data
  grrlib
  ${FAT}
  ${PNG_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  inipp::inipp
)

ogc_create_dol(MiisendU-Wii)
