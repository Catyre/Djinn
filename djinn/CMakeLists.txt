# Build Djinn library
file(STRINGS "djinn_lib.txt" DJINN_LIB_RAW)
set(DJINN_LIB "")

foreach(djinn_file_line ${DJINN_LIB_RAW})
  string(CONFIGURE ${djinn_file_line} djinn_file_line @ONLY)
  list(APPEND DJINN_LIB ${djinn_file_line})
endforeach(djinn_file_line ${DJINN_LIB_RAW})

add_library(djinn STATIC ${DJINN_LIB})
set_target_properties(djinn PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/lib/djinn")
install(TARGETS djinn DESTINATION ${CMAKE_CURRENT_LIST_DIR}/lib/djinn/)
