# SPDX-License-Identifier: AGPL-3.0-or-later
cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED HTML_TEMPLATE_ROOT)
  message(FATAL_ERROR "Need -DHTML_TEMPLATE_ROOT=...")
endif()

file(GLOB children LIST_DIRECTORIES true "${HTML_TEMPLATE_ROOT}/*")
foreach(d IN LISTS children)
  if(IS_DIRECTORY "${d}")
    file(REMOVE_RECURSE "${d}/js" "${d}/css" "${d}/assets")
    file(REMOVE "${d}/index.html")
  endif()
endforeach()
