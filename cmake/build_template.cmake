# SPDX-License-Identifier: AGPL-3.0-or-later
cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED INPUT_HTML OR NOT DEFINED OUTPUT_HTML OR NOT DEFINED ROOT_DIR)
  message(FATAL_ERROR "Usage: -DINPUT_HTML=... -DOUTPUT_HTML=... -DROOT_DIR=...")
endif()

file(READ "${INPUT_HTML}" html)

# ---------------------------
# INLINE CSS markers
# ---------------------------
string(REGEX MATCHALL "<!--[ \t]*INLINE_CSS:[ \t]*[^>]+[ \t]*-->" css_markers "${html}")
foreach(m IN LISTS css_markers)
  string(REGEX REPLACE "^<!--[ \t]*INLINE_CSS:[ \t]*" "" rel "${m}")
  string(REGEX REPLACE "[ \t]*-->$" "" rel "${rel}")
  string(STRIP "${rel}" rel)

  set(path "${ROOT_DIR}/${rel}")
  if(NOT EXISTS "${path}")
    message(FATAL_ERROR "CSS file not found: ${path} (from marker: ${m})")
  endif()

  file(READ "${path}" content)
  set(repl "<style>\n/* ---- ${rel} (inlined) ---- */\n${content}\n</style>")
  string(REPLACE "${m}" "${repl}" html "${html}")
endforeach()

# ---------------------------
# INLINE JS markers
# Looks under ${ROOT_DIR}/js/<file>
# ---------------------------
string(REGEX MATCHALL "<!--[ \t]*INLINE_JS:[ \t]*[^>]+[ \t]*-->" js_markers "${html}")
foreach(m IN LISTS js_markers)
  string(REGEX REPLACE "^<!--[ \t]*INLINE_JS:[ \t]*" "" rel "${m}")
  string(REGEX REPLACE "[ \t]*-->$" "" rel "${rel}")
  string(STRIP "${rel}" rel)

  set(path "${ROOT_DIR}/js/${rel}")
  if(NOT EXISTS "${path}")
    message(FATAL_ERROR "JS file not found: ${path} (from marker: ${m})")
  endif()

  file(READ "${path}" content)
  set(repl "<script>\n/* ---- js/${rel} (inlined) ---- */\n${content}\n</script>")
  string(REPLACE "${m}" "${repl}" html "${html}")
endforeach()

# ---------------------------
# INLINE IMG markers (AFTER CSS/JS)
# File contains base64 (or whatever text you want injected).
# ---------------------------
string(REGEX MATCHALL "<!--[ \t]*INLINE_IMG:[ \t]*[^>]+[ \t]*-->" img_markers "${html}")
foreach(m IN LISTS img_markers)
  string(REGEX REPLACE "^<!--[ \t]*INLINE_IMG:[ \t]*" "" rel "${m}")
  string(REGEX REPLACE "[ \t]*-->$" "" rel "${rel}")
  string(STRIP "${rel}" rel)

  set(path "${ROOT_DIR}/${rel}")
  if(NOT EXISTS "${path}")
    message(FATAL_ERROR "IMG base64 file not found: ${path} (from marker: ${m})")
  endif()

  file(READ "${path}" content)
  string(STRIP "${content}" content)

  string(REPLACE "${m}" "${content}" html "${html}")
endforeach()

file(WRITE "${OUTPUT_HTML}" "${html}")
message(VERBOSE "Generated ${OUTPUT_HTML}")
