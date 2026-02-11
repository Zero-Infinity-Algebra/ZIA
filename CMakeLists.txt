cmake_minimum_required(VERSION 3.15)
project(ziavalue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ZIA_BUILD_PYTHON "Build Python module (pybind11)" ON)

# Core ZIA library
add_library(ZIACore STATIC
    src/ZIAValue.cpp
)

target_include_directories(ZIACore
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Formal unit tests (doctest)
add_executable(zia_unit_tests
    tests/test_ziavalue.cpp
)
target_link_libraries(zia_unit_tests PRIVATE ZIACore)
target_include_directories(zia_unit_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

enable_testing()
add_test(NAME zia_unit_tests COMMAND zia_unit_tests)

# Python bindings (optional)
if(ZIA_BUILD_PYTHON)
  find_package(Python 3.13 REQUIRED COMPONENTS Interpreter Development)

  # Ask *that* Python where pybind11's CMake package lives
  execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE _pybind11_cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if(NOT _pybind11_cmake_dir)
    message(FATAL_ERROR "pybind11 not found in ${Python_EXECUTABLE}. Install it (e.g. pip install pybind11).")
  endif()

  # Make CMake aware of it
  list(PREPEND CMAKE_PREFIX_PATH "${_pybind11_cmake_dir}")

  find_package(pybind11 CONFIG REQUIRED)

  pybind11_add_module(zia
      bindings/zia_python.cpp
  )
  target_link_libraries(zia PRIVATE ZIACore)
endif()
