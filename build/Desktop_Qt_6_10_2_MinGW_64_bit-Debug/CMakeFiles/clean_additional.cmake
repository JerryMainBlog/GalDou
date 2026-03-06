# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "editor\\CMakeFiles\\GalDou_Editor_autogen.dir\\AutogenUsed.txt"
  "editor\\CMakeFiles\\GalDou_Editor_autogen.dir\\ParseCache.txt"
  "editor\\GalDou_Editor_autogen"
  "engine\\CMakeFiles\\GalDou_Engine_autogen.dir\\AutogenUsed.txt"
  "engine\\CMakeFiles\\GalDou_Engine_autogen.dir\\ParseCache.txt"
  "engine\\GalDou_Engine_autogen"
  "runner\\CMakeFiles\\GalDou_autogen.dir\\AutogenUsed.txt"
  "runner\\CMakeFiles\\GalDou_autogen.dir\\ParseCache.txt"
  "runner\\GalDou_autogen"
  )
endif()
