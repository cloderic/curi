find_package(Doxygen)
include(CMakeDependentOption)

# Options
cmake_dependent_option(GENERATE_DOCUMENTATION "Generate the user documentation using doxygen"
    ON "DOXYGEN_FOUND"
    OFF)

if(GENERATE_DOCUMENTATION)
  # Doxygen target setup
  set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
  set(DOXYGEN_HTML_OUTPUT_DIR html)
  set(DOXYGEN_CONFIGURED_INPUT ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)
  configure_file(curi.dox.in ${DOXYGEN_CONFIGURED_INPUT} @ONLY)

  add_custom_target(doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
    COMMENT "User documentation generation"
    SOURCES curi.dox.in
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}/${DOXYGEN_HTML_OUTPUT_DIR}/ DESTINATION ./doc)
endif()