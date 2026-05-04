# SPDX-License-Identifier: AGPL-3.0-or-later
include_guard(GLOBAL)

function(tucu_package_html_templates OUT_TARGET)
  set(oneValueArgs
    SOURCE_DIR
    DEST_DIR
    INLINE_SCRIPT
    CLEAN_SCRIPT
  )
  set(multiValueArgs)
  cmake_parse_arguments(TUCU "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT TUCU_SOURCE_DIR OR NOT TUCU_DEST_DIR OR NOT TUCU_INLINE_SCRIPT OR NOT TUCU_CLEAN_SCRIPT)
    message(FATAL_ERROR
      "tucu_package_html_templates(${OUT_TARGET}) requires:\n"
      "  SOURCE_DIR <dir>\n"
      "  DEST_DIR <dir>\n"
      "  INLINE_SCRIPT <file.cmake>\n"
      "  CLEAN_SCRIPT <file.cmake>\n"
    )
  endif()

  add_custom_target(${OUT_TARGET}_copy
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${TUCU_SOURCE_DIR}
            ${TUCU_DEST_DIR}
    COMMENT "Copying HTML templates to build directory"
    VERBATIM
  )

  file(GLOB TEMPLATE_CANDIDATES RELATIVE
    "${TUCU_SOURCE_DIR}"
    "${TUCU_SOURCE_DIR}/*"
  )

  set(ALL_INLINED_OUTPUTS "")
  foreach(t IN LISTS TEMPLATE_CANDIDATES)
    if(IS_DIRECTORY "${TUCU_SOURCE_DIR}/${t}")
      set(TPL_SRC_DIR "${TUCU_SOURCE_DIR}/${t}")
      set(TPL_DST_DIR "${TUCU_DEST_DIR}/${t}")

      set(INPUT_HTML  "${TPL_DST_DIR}/index.html")
      set(OUTPUT_HTML "${TPL_DST_DIR}/index_js.html")

      file(GLOB_RECURSE TPL_SRC_FILES "${TPL_SRC_DIR}/*")

      add_custom_command(
        OUTPUT "${OUTPUT_HTML}"
        COMMAND ${CMAKE_COMMAND}
                -DINPUT_HTML:FILEPATH=${INPUT_HTML}
                -DOUTPUT_HTML:FILEPATH=${OUTPUT_HTML}
                -DROOT_DIR:PATH=${TPL_DST_DIR}
                -P ${TUCU_INLINE_SCRIPT}
        DEPENDS
                ${OUT_TARGET}_copy
                ${TPL_SRC_FILES}
                ${TUCU_INLINE_SCRIPT}
        COMMENT "Built ${t}/index_js.html (inlined CSS/JS/IMG)"
        VERBATIM
      )

      list(APPEND ALL_INLINED_OUTPUTS "${OUTPUT_HTML}")
    endif()
  endforeach()

  add_custom_target(${OUT_TARGET}_inline
    DEPENDS ${ALL_INLINED_OUTPUTS}
  )
  add_dependencies(${OUT_TARGET}_inline ${OUT_TARGET}_copy)

  add_custom_target(${OUT_TARGET}_clean
    COMMAND ${CMAKE_COMMAND}
            -DHTML_TEMPLATE_ROOT:PATH=${TUCU_DEST_DIR}
            -P ${TUCU_CLEAN_SCRIPT}
    COMMENT "Cleaning inlined templates (remove js/css/index.html)"
    VERBATIM
  )
  add_dependencies(${OUT_TARGET}_clean ${OUT_TARGET}_inline)

  add_custom_target(${OUT_TARGET} DEPENDS ${OUT_TARGET}_clean)
endfunction()
