# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\blazestudioprovs_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\blazestudioprovs_autogen.dir\\ParseCache.txt"
  "blazestudioprovs_autogen"
  )
endif()
