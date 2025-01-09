cmake_minimum_required(VERSION 3.13)

function(add_embedded_tpl target)
  if (NOT ${ARGC} GREATER 1)
    message(FATAL_ERROR "add_embedded_tpl: must provide at least one input file")
  endif()

  if (NOT GXTEXCONV_EXE)
    message(FATAL_ERROR "Could not find gxtexconv: try installing general-tools")
  endif()

  if (NOT DKP_BIN2S)
    message(FATAL_ERROR "Could not find bin2s: try installing general-tools")
  endif()

  __dkp_asm_lang(lang add_embedded_tpl)
  set(genfolder "${CMAKE_CURRENT_BINARY_DIR}/.dkp-generated/${target}")
  set(intermediates "")
  foreach (inname IN LISTS ARGN)
    dkp_resolve_file(infiles "${inname}" MULTI)
    foreach(infile IN LISTS infiles)
      get_filename_component(basename "${infile}" NAME_WE)
      string(REPLACE "." "_" basename "${basename}")

      if (TARGET "${inname}")
        set(indeps ${inname} ${infile})
      else()
        set(indeps ${infile})
      endif()

      add_custom_command(
        OUTPUT "${genfolder}/${basename}_tpl.S" "${genfolder}/${basename}_tpl.h" "${genfolder}/${basename}.h"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${genfolder}"
        COMMAND ${GXTEXCONV_EXE} -s "${infile}" -d "${genfolder}/${basename}.d" -o "${genfolder}/${basename}.tpl"
        COMMAND ${DKP_BIN2S} -a ${DKP_BIN2S_ALIGNMENT} -H "${genfolder}/${basename}_tpl.h" "${genfolder}/${basename}.tpl" > "${genfolder}/${basename}_tpl.S"
        DEPENDS ${indeps}
        COMMENT "Generating binary embedding source for ${inname}"
      )

      list(APPEND intermediates "${genfolder}/${basename}_tpl.S" "${genfolder}/${basename}_tpl.h")
      set_source_files_properties("${genfolder}/${basename}_tpl.S" PROPERTIES LANGUAGE "${lang}")
    endforeach()
  endforeach()

  add_library(${target} OBJECT ${intermediates})
  target_include_directories(${target} INTERFACE "${genfolder}")
endfunction()
