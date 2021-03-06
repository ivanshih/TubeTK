##############################################################################
#
# Library:   TubeTK
#
# Copyright 2010 Kitware Inc. 28 Corporate Drive,
# Clifton Park, NY, 12065, USA.
#
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################

project( TubeTK )

cmake_minimum_required( VERSION 2.8.7 )
if( NOT ${CMAKE_VERSION} VERSION_LESS 2.8.13 )
  cmake_policy( SET CMP0026 OLD )
endif( NOT ${CMAKE_VERSION} VERSION_LESS 2.8.13 )

# Update CMake module path
set( TubeTK_CMAKE_EXTENSIONS_DIR ${TubeTK_SOURCE_DIR}/CMake )
set( CMAKE_MODULE_PATH ${TubeTK_CMAKE_EXTENSIONS_DIR} ${CMAKE_MODULE_PATH} )

#
# Set TubeTK version and output directories.
#
set( TubeTK_VERSION_MAJOR "0" )
set( TubeTK_VERSION_MINOR "9" )
set( TubeTK_VERSION_PATCH "0" )

include( TubeTKFunctionToday )
TubeTKFunctionToday( TubeTK_VERSION_BUILD )

set( TubeTK_VERSION
  "${TubeTK_VERSION_MAJOR}.${TubeTK_VERSION_MINOR}.${TubeTK_VERSION_PATCH}.${TubeTK_VERSION_BUILD}" )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/TubeTK" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/TubeTK" )

include( TubeTKMacroEmptyExternalProject )

#
# CDash dashboard setup.
#
include( CTest )
if( BUILD_TESTING )
  set( BUILDNAME "${BUILDNAME}" CACHE STRING
    "Name of the build on the CDash dashboard." )
  mark_as_advanced( BUILDNAME )
endif( BUILD_TESTING )

set( BUILD_WARNING_REPORT_LIMIT -1 )
set( BUILD_ERROR_REPORT_LIMIT -1 )

include( CTestConfig.cmake )

configure_file( ${TubeTK_SOURCE_DIR}/CMake/CTestCustom.cmake.in
  ${TubeTK_BINARY_DIR}/CTestCustom.cmake )

#
# Configure for the download of CTest unit testing data from Midas
#
set( MIDAS_USER "" CACHE STRING "Midas Platform user email address." )
mark_as_advanced( MIDAS_USER )
set( MIDAS_DEFAULT_API_KEY "" CACHE STRING "Midas Platform default API key." )
mark_as_advanced( MIDAS_DEFAULT_API_KEY )
set( MIDAS_DATA_DIR ${TubeTK_BINARY_DIR}/MIDAS_Data )

#
# Doxygen documentation setup.
#
option( BUILD_DOCUMENTATION
  "Build the software guide and source code documentation." OFF )
if( BUILD_DOCUMENTATION )
  option( TubeTK_USE_DOXYGEN
    "Use Doxygen to generate the source code documentation." ON )
  mark_as_advanced( TubeTK_USE_DOXYGEN )
  if( TubeTK_USE_DOXYGEN )
    add_subdirectory( CMake/Doxygen )
  endif( TubeTK_USE_DOXYGEN )
endif( BUILD_DOCUMENTATION )

#
# Image viewer setup.
#
option( TubeTK_BUILD_IMAGE_VIEWER "Use ImageViewer." OFF )

#
# Microsoft Visual Studio setup.
#
if( MSVC )
  add_definitions( -D_SCL_SECURE_NO_WARNINGS )
  if( NOT CMAKE_FIRST_PASS_COMPLETED AND "${CMAKE_CXX_COMPILER}" MATCHES "cl" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /MP"
         CACHE STRING "Flags used by all build types." FORCE )
  endif( NOT CMAKE_FIRST_PASS_COMPLETED AND "${CMAKE_CXX_COMPILER}" MATCHES "cl" )
  option( DISABLE_MSVC8_DEPRECATED_WARNINGS
    "Disable Microsoft Visual Studio 8 deprecated warnings." ON )
  mark_as_advanced( FORCE DISABLE_MSVC8_DEPRECATED_WARNINGS )
  if( DISABLE_MSVC8_DEPRECATED_WARNINGS )
    add_definitions( -D_CRT_SECURE_NO_DEPRECATE )
  endif( DISABLE_MSVC8_DEPRECATED_WARNINGS )
endif( MSVC )

#
# Apple OS X setup.
#
if( APPLE )
  include( TubeTKBlockSetCMakeOSXVariables )
  if( NOT CMAKE_FIRST_PASS_COMPLETED )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common"
      CACHE STRING "Flags used by all build types." FORCE )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common"
      CACHE STRING "Flags used by all build types." FORCE )
  endif( NOT CMAKE_FIRST_PASS_COMPLETED )
endif( APPLE )

#
# Linux or UNIX setup.
#
if( UNIX )
  if( NOT CMAKE_FIRST_PASS_COMPLETED )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing"
      CACHE STRING "Flags used by all build types." FORCE )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing"
      CACHE STRING "Flags used by all build types." FORCE )
    if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
      set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC"
           CACHE STRING "Flags used by all build types." FORCE )
      set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC"
           CACHE STRING "Flags used by all build types." FORCE )
    endif( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
  endif( NOT CMAKE_FIRST_PASS_COMPLETED )
endif( UNIX )

#
# Implementation agnostic fPIC
#
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

#
# Build TubeTK to operate as a Slicer module
#
option( TubeTK_BUILD_SLICER_MODULES
  "Use Slicer to extract useful commands and CLIs." OFF )
if( TubeTK_BUILD_SLICER_MODULES )
  include( TubeTKSlicer4ExtensionConfig )

  mark_as_advanced( TubeTK_USE_ITK TubeTK_USE_VTK TubeTK_USE_QT
    TubeTK_USE_CTK )

  unset( QT_QMAKE_EXECUTABLE )
  unset( PYTHON_EXECUTABLE )

  # Find Slicer to set the [ITK|VTK|CTK|...]_DIR variables
  find_package( Slicer REQUIRED )
  if( NOT Slicer_USE_FILE )
    message( FATAL "Slicer_USE_FILE not defined. Find Slicer silently failed." )
  endif( NOT Slicer_USE_FILE )
  include( ${Slicer_USE_FILE} )

  if( Slicer_SOURCE_DIR )
    set( TubeTK_SLICER_SOURCE_DIR "${Slicer_SOURCE_DIR}" )
  else( Slicer_SOURCE_DIR )
    set( TubeTK_SLICER_SOURCE_DIR "${Slicer_CMAKE_DIR}/.." )
  endif( Slicer_SOURCE_DIR )

  if( Slicer_BINARY_DIR )
    set( TubeTK_SLICER_BINARY_DIR "${Slicer_BINARY_DIR}" )
  else( Slicer_BINARY_DIR )
    set( TubeTK_SLICER_BINARY_DIR "${Slicer_HOME}" )
  endif( Slicer_BINARY_DIR )

  # All those dependencies are already provided by Slicer
  set( USE_SYSTEM_ITK ON CACHE BOOL
    "Exclude ITK from superbuild and use an existing build." FORCE )
  set( TubeTK_USE_VTK ON CACHE BOOL
    "Use VTK to enable additional functionality." FORCE )
  set( USE_SYSTEM_VTK ON CACHE BOOL
    "Exclude VTK from superbuild and use an existing build." FORCE )
  set( TubeTK_USE_QT ON CACHE BOOL
    "Use the Qt framework to enable additional functionality." FORCE )
  set( USE_SYSTEM_CTK ON CACHE BOOL
    "Exclude CTK from superbuild and use an existing build." FORCE )
  set( USE_SYSTEM_SLICER_EXECUTION_MODEL ON CACHE BOOL
    "Exclude SlicerExecutionModel from superbuild and use an existing build."
    FORCE )
  mark_as_advanced( CTK_DIR DCMTK_DIR ITK_DIR LibArchive_INCLUDE_DIR
    LibArchive_LIBRARY OpenIGTLink_DIR QT_QMAKE_EXECUTABLE QtTesting_DIR
    SlicerExecutionModel_DIR Slicer_EXTENSIONS_TRACK_QUALIFIER Teem_DIR
    VTK_DIR ZLIB_INCLUDE_DIR ZLIB_LIBRARY ZLIB_ROOT jqPlot_DIR qRestAPI_DIR
    CURL_INCLUDE_DIR CURL_LIBRARY ZLIB_LIBRARY ZLIB_ROOT ZLIB_INCLUDE_DIR
    jpPlot_DIR MIDAS_PACKAGE_API_KEY MIDAS_PACKAGE_EMAIL MIDAS_PACKAGE_URL )
else( TubeTK_BUILD_SLICER_MODULES )
  mark_as_advanced( CLEAR TubeTK_USE_ITK TubeTK_USE_VTK TubeTK_USE_QT )
  if( TubeTK_USE_QT )
    mark_as_advanced( CLEAR TubeTK_USE_CTK )
  endif( TubeTK_USE_QT )
  mark_as_advanced( Slicer_DIR )
endif( TubeTK_BUILD_SLICER_MODULES )

include( TubeTKSlicer4Config )

#
# Boost libraries setup.
#
option( TubeTK_USE_BOOST
  "Use the Boost libraries to enable additional functionality." OFF )
if( TubeTK_USE_BOOST )
  set( Boost_USE_STATIC_LIBS ON )
  set( Boost_USE_MULTITHREADED ON )
  set( Boost_USE_STATIC_RUNTIME OFF )
  find_package( Boost REQUIRED )
  if( NOT Boost_FOUND )
    set( BOOST_ROOT "NOT_FOUND" CACHE PATH
      "Path to the top-level directory of Boost." FORCE )
    mark_as_advanced( CLEAR BOOST_ROOT )
  endif( NOT Boost_FOUND )
endif( TubeTK_USE_BOOST )


#
#  Cppcheck setup.
#
set( TubeTK_USE_CPPCHECK_DEFAULT ON )
set( USE_SYSTEM_CPPCHECK_DEFAULT OFF )
if( WIN32 )
  set( TubeTK_USE_CPPCHECK_DEFAULT OFF )
  set( USE_SYSTEM_CPPCHECK_DEFAULT OFF )
endif( WIN32 )
option( TubeTK_USE_CPPCHECK "Use Cppcheck to enable additional tests."
  ${TubeTK_USE_CPPCHECK_DEFAULT} )
option( USE_SYSTEM_CPPCHECK
  "Exclude Cppcheck from superbuild and use an existing build."
  ${USE_SYSTEM_CPPCHECK_DEFAULT} )
mark_as_advanced( USE_SYSTEM_CPPCHECK )

#
# Setup python
#
set( TubeTK_USE_PYTHON_DEFAULT ON )
if( WIN32 OR TubeTK_BUILD_SLICER_MODULES )
  set( TubeTK_USE_PYTHON_DEFAULT OFF )
endif( WIN32 OR TubeTK_BUILD_SLICER_MODULES )
option( TubeTK_USE_PYTHON "Use Python to enable additional functionality."
  ${TubeTK_USE_PYTHON_DEFAULT} )
if( TubeTK_USE_PYTHON )
  # NumPy conversion support.
  option( TubeTK_USE_NUMPY "Use NumPy to enable additional functionality." ON )
  mark_as_advanced( CLEAR TubeTK_USE_NUMPY )

  # Test TubeTK examples via Python virtualenv.
  option( TubeTK_USE_NOTEBOOKS
    "Test IPython notebooks in a Python virtualenv." ON )
  mark_as_advanced( CLEAR TubeTK_USE_NOTEBOOKS )

  # PyQtGraph bridge support.
  option( TubeTK_USE_PYQTGRAPH
    "Use PyQtGraph to enable additional functionality." ON )
  mark_as_advanced( CLEAR TubeTK_USE_PYQTGRAPH )
else( TubeTK_USE_PYTHON )
  # NumPy conversion support.
  set( TubeTK_USE_NUMPY OFF CACHE BOOL
    "Use NumPy to enable additional functionality." FORCE )
  mark_as_advanced( TubeTK_USE_NUMPY )

  # Test TubeTK examples via Python virtualenv.
  set( TubeTK_USE_NOTEBOOKS OFF CACHE BOOL
    "Test IPython notebooks in a Python virtualenv." FORCE )
  mark_as_advanced( TubeTK_USE_NOTEBOOKS )

  # PyQtGraph bridge support.
  set( TubeTK_USE_PYQTGRAPH OFF CACHE BOOL
    "Use PyQtGraph to enable additional functionality." FORCE )
  mark_as_advanced( TubeTK_USE_PYQTGRAPH )
endif( TubeTK_USE_PYTHON )

#
# ITK setup.
#
option( USE_SYSTEM_ITK "Exclude ITK from superbuild and use an existing build."
  OFF )
mark_as_advanced( USE_SYSTEM_ITK )

#
# JsonCpp library setup.
#
option( USE_SYSTEM_JSONCPP
  "Exclude JsonCpp from superbuild and use an existing build." OFF )
mark_as_advanced( USE_SYSTEM_JSONCPP )

#
# KWStyle setup.
#
option( TubeTK_USE_KWSTYLE
  "Use KWStyle to check for coding standards violations." ON )
option( USE_SYSTEM_KWSTYLE
  "Exclude KWStyle from superbuild and use an existing build." OFF )
mark_as_advanced( USE_SYSTEM_KWSTYLE )

#
# LIBSVM setup.
#
if( MSVC )
  option( TubeTK_USE_LIBSVM "Use LIBSVM to enable additional functionality." OFF )
else( MSVC )
  option( TubeTK_USE_LIBSVM "Use LIBSVM to enable additional functionality." ON )
endif( MSVC )
option( USE_SYSTEM_LIBSVM
  "Exclude LIBSVM from superbuild and use an existing build." OFF )
mark_as_advanced( USE_SYSTEM_LIBSVM )

#
# Parameter serializer setup.
#
option( USE_SYSTEM_SLICER_EXECUTION_MODEL
  "Exclude slicer execution model from superbuild and use an existing build."
  OFF )
mark_as_advanced( USE_SYSTEM_SLICER_EXECUTION_MODEL )
option( USE_SYSTEM_PARAMETER_SERIALIZER
  "Exclude parameter serializer from superbuild and use an existing build."
  OFF )
mark_as_advanced( USE_SYSTEM_PARAMETER_SERIALIZER )

#
# Qt framework setup.
#
option( TubeTK_USE_QT
  "Use the Qt framework to enable additional applications and libraries." ON )
if( TubeTK_USE_QT )
  # CTK setup.
  option( TubeTK_USE_CTK "Use CTK to enable additional functionality." ON )
  if( NOT TubeTK_BUILD_SLICER_MODULES )
    mark_as_advanced( CLEAR TubeTK_USE_CTK )
  endif( NOT TubeTK_BUILD_SLICER_MODULES )
  option( USE_SYSTEM_CTK
    "Exclude CTK from superbuild and use an existing build." OFF )
  if( NOT TubeTK_BUILD_SLICER_MODULES )
    mark_as_advanced( USE_SYSTEM_CTK )
  endif( NOT TubeTK_BUILD_SLICER_MODULES )

  find_package( Qt4 )
  # 3D Slicer only tests against a specific Qt 4 version. To
  # override the version check, set the TubeTK_REQUIRED_QT_VERSION CMake
  # variable.
  if( NOT DEFINED TubeTK_REQUIRED_QT_VERSION )
    set( TubeTK_REQUIRED_QT_VERSION "4.7.4" CACHE STRING
      "Exact Qt4 framework version required." FORCE )
    mark_as_advanced( TubeTK_REQUIRED_QT_VERSION )
  endif( NOT DEFINED TubeTK_REQUIRED_QT_VERSION )
  if( QT4_FOUND )
    if( "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
        VERSION_LESS "${TubeTK_REQUIRED_QT_VERSION}" )
      message( FATAL_ERROR
        "error: TubeTK requires at least Qt ${TubeTK_REQUIRED_QT_VERSION} "
        "- you have ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
        " ${extra_error_message}" )
    endif( "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}"
      VERSION_LESS "${TubeTK_REQUIRED_QT_VERSION}" )
    include( ${QT_USE_FILE} )
  else( QT4_FOUND )
    message( SEND_ERROR "Error: Qt4 framework not found." )
  endif( QT4_FOUND )
else( TubeTK_USE_QT )
  option( TubeTK_USE_CTK "Use CTK to enable additional functionality." OFF )
  mark_as_advanced( TubeTK_USE_CTK )
  option( USE_SYSTEM_CTK
    "Exclude CTK from superbuild and use an existing build." OFF )
  mark_as_advanced( USE_SYSTEM_CTK )
endif( TubeTK_USE_QT )

#
# Slicer Execution model Setup
#
option( USE_SYSTEM_SLICER_EXECUTION_MODEL
  "Exclude SlicerExecutionModel from superbuild and use an existing build."
  OFF )
mark_as_advanced( USE_SYSTEM_SLICER_EXECUTION_MODEL )

option( TubeTK_USE_VALGRIND "Use Valgrind to enable additional functionality."
  OFF )

#
# VTK setup.
#
option( TubeTK_USE_VTK "Use VTK to enable additional functionality." ON )
option( USE_SYSTEM_VTK "Exclude VTK from superbuild and use an existing build."
  OFF )
mark_as_advanced( USE_SYSTEM_VTK )

#
# Modules
#
option( TubeTK_BUILD_APPLICATIONS
  "Build applications. If not, only TubeTK library is built" ON )

#
# Indicate that this CMakeLists.txt file has already been parsed once. Keeps
# append commands from having an effect every time CMake is run.
#
set( CMAKE_FIRST_PASS_COMPLETED ON CACHE BOOL
  "Modify the cache variables only on the first pass through the CMake file."
  FORCE )
mark_as_advanced( CMAKE_FIRST_PASS_COMPLETED )

# If TubeTK_CONFIG_BINARY_DIR isn't defined, it means TubeTK is *NOT* build
# using Superbuild. In that specific case, TubeTK_CONFIG_BINARY_DIR should
# default to TubeTK_BINARY_DIR
if( NOT DEFINED TubeTK_CONFIG_BINARY_DIR )
  set( TubeTK_CONFIG_BINARY_DIR ${CMAKE_BINARY_DIR} )
endif( NOT DEFINED TubeTK_CONFIG_BINARY_DIR )

#
# Setup Superbuild
#
option( TubeTK_USE_SUPERBUILD
  "Build TubeTK and the projects it depends on using a superbuild." ON )
mark_as_advanced( TubeTK_USE_SUPERBUILD )

if( TubeTK_USE_SUPERBUILD )
  if( TubeTK_BUILD_APPLICATIONS )
    # Expose the TubeTK_BUILD_* variables to Superbuild
    file( GLOB_RECURSE TubeTK_modules_files "TubeTKModules.cmake" )
    foreach( f ${TubeTK_modules_files} )
      include( ${f} )
    endforeach( f ${TubeTK_modules_files} )
  endif( TubeTK_BUILD_APPLICATIONS )

  add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Superbuild" )
  return()
endif( TubeTK_USE_SUPERBUILD )

#
# Once superbuild is done, build TubeTK
#
set( TubeTK_INSTALL_BIN_DIR "bin" )
set( TubeTK_INSTALL_LIB_DIR "lib/TubeTK" )
set( TubeTK_INSTALL_INCLUDE_DIR "include/TubeTK" )
set( TubeTK_INSTALL_SHARE_DIR "share/TubeTK" )

set( TubeTK_INSTALL_PLUGINS_BIN_DIR "${TubeTK_INSTALL_BIN_DIR}" )
set( TubeTK_INSTALL_PLUGINS_CACHE_DIR "${TubeTK_INSTALL_LIB_DIR}/CACHE" )
set( TubeTK_INSTALL_PLUGINS_INCLUDE_DIR "${TubeTK_INSTALL_INCLUDE_DIR}" )
set( TubeTK_INSTALL_PLUGINS_LIB_DIR "${TubeTK_INSTALL_LIB_DIR}" )
set( TubeTK_INSTALL_PLUGINS_SHARE_DIR "${TubeTK_INSTALL_SHARE_DIR}" )

if( TubeTK_USE_CPPCHECK )
  list( APPEND CMAKE_MODULE_PATH "${TubeTK_SOURCE_DIR}/CMake/Cppcheck" )
  find_package( Cppcheck REQUIRED )
  set( CPPCHECK_PATHS_TO_CHECK "${TubeTK_SOURCE_DIR}/Applications"
    "${TubeTK_SOURCE_DIR}/Base" )
  set( CPPCHECK_SUPPRESSIONS_FILE
    "${TubeTK_SOURCE_DIR}/CMake/Cppcheck/TubeTKCppcheckSuppressions.txt" )
  set( CPPCHECK_COMMAND_PREFIX "${CPPCHECK_EXECUTABLE}" -q --xml --suppressions
    "${CPPCHECK_SUPPRESSIONS_FILE}" )
  add_test( Cppcheck
    ${CPPCHECK_COMMAND_PREFIX} --enable=all ${CPPCHECK_PATHS_TO_CHECK} )
  set_tests_properties( Cppcheck
    PROPERTIES FAIL_REGULAR_EXPRESSION "error file=" )
endif( TubeTK_USE_CPPCHECK )

if( TubeTK_USE_QT )
  if( TubeTK_USE_CTK )
    find_package( CTK REQUIRED )
    include( ${CTK_USE_FILE} )
  endif( TubeTK_USE_CTK )
endif( TubeTK_USE_QT )

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

find_package( JsonCpp REQUIRED )
include_directories( ${JsonCpp_INCLUDE_DIRS} )

if( TubeTK_USE_KWSTYLE )
  include( TubeTKKWStyleConfig )
endif( TubeTK_USE_KWSTYLE )

if( TubeTK_USE_LIBSVM )
  find_package( LIBSVM REQUIRED )
endif( TubeTK_USE_LIBSVM )

if( TubeTK_USE_BOOST )
  set( Boost_USE_STATIC_LIBS ON )
  set( Boost_USE_MULTITHREADED ON )
  set( Boost_USE_STATIC_RUNTIME OFF )
  find_package( Boost REQUIRED )
endif( TubeTK_USE_BOOST )

find_package( ParameterSerializer REQUIRED )
include_directories( ${ParameterSerializer_INCLUDE_DIRS} )

set( SlicerExecutionModel_CLI_RUNTIME_INSTALL_DIRECTORY
  ${TubeTK_INSTALL_BIN_DIR} )
set( SlicerExecutionModel_CLI_LIBRARY_INSTALL_DIRECTORY
  ${TubeTK_INSTALL_LIB_DIR} )
set( SlicerExecutionModel_CLI_INCLUDE_INSTALL_DIRECTORY
  ${TubeTK_INSTALL_INCLUDE_DIR} )
set( SlicerExecutionModel_CLI_SHARE_INSTALL_DIRECTORY
  ${TubeTK_INSTALL_SHARE_DIR} )
set( SlicerExecutionModel_CLI_ARCHIVE_INSTALL_DIRECTORY
  ${TubeTK_INSTALL_LIB_DIR} )

set( SlicerExecutionModel_CLI_RUNTIME_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/bin" )
set( SlicerExecutionModel_CLI_LIBRARY_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/lib/TubeTK" )
set( SlicerExecutionModel_CLI_INCLUDE_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/include/TubeTK" )
set( SlicerExecutionModel_CLI_SHARE_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/share/TubeTK" )
set( SlicerExecutionModel_CLI_ARCHIVE_OUTPUT_DIRECTORY
  "${CMAKE_BINARY_DIR}/lib/TubeTK" )

find_package( SlicerExecutionModel REQUIRED GenerateCLP )
include( ${GenerateCLP_USE_FILE} )
include( ${SlicerExecutionModel_DIR}/CMake/SEMMacroBuildCLI.cmake )

if( TubeTK_USE_VTK )
  find_package( VTK REQUIRED )
  include( ${VTK_USE_FILE} )
endif( TubeTK_USE_VTK )

# Check endianness.
include( TestBigEndian )
test_big_endian( CMAKE_WORDS_BIGENDIAN )

#
# Setup Launcher
#

# Set TubeTK_EXECUTABLE_DIRS useful to configure launcher scripts
# and set TubeTK_LIBRARY_DIRS
if( MSVC )
  set(TubeTK_EXECUTABLE_DIRS CACHE INTERNAL "Bin and Lib dirs for running apps." FORCE )
  foreach(_build_type "" Debug Release)
    list( APPEND TubeTK_EXECUTABLE_DIRS
      ${JsonCpp_DIR}/bin/${_build_type}
      ${ParameterSerializer_DIR}/bin/${_build_type}
      ${GenerateCLP_DIR}/${_build_type}
      ${Boost_LIBRARY_DIR}
      ${ITK_DIR}/bin/${_build_type}
      ${TubeTK_BINARY_DIR}/bin/${_build_type}
      ${TubeTK_BINARY_DIR}/lib/TubeTK/Plugins/${_build_type} )
    if( TubeTK_USE_VTK )
      list( APPEND TubeTK_EXECUTABLE_DIRS
        ${VTK_DIR}/bin/${_build_type} )
    endif( TubeTK_USE_VTK )
    if( TubeTK_BUILD_SLICER_MODULES )
      list( APPEND TubeTK_EXECUTABLE_DIRS
        ${Slicer_DIR}/bin/${_build_type} )
    endif( TubeTK_BUILD_SLICER_MODULES )
  endforeach()
else( MSVC )
  set( TubeTK_EXECUTABLE_DIRS
    ${JsonCpp_DIR}/bin
    ${ParameterSerializer_DIR}/bin
    ${GenerateCLP_DIR}
    ${Boost_LIBRARY_DIR}
    ${ITK_DIR}/bin
    ${TubeTK_BINARY_DIR}/bin
    ${TubeTK_BINARY_DIR}/lib
    ${TubeTK_BINARY_DIR}/lib/TubeTK/Plugins
    CACHE INTERNAL "Bin and Lib dirs for running apps." FORCE )
  if( TubeTK_USE_VTK )
    list( APPEND ${VTK_DIR}/bin )
  endif( TubeTK_USE_VTK )
endif( MSVC )

#
# Configure a launcher for running TubeTK methods from the command line
#
message(STATUS "Configuring Launcher script")
set(TubeTK_LAUNCHER )
if( WIN32 )
  # Microsoft Windows.
  set(_launcher_platform "windows")
  configure_file( ${TubeTK_SOURCE_DIR}/CMake/TubeTKLauncher.bat.in
    ${TubeTK_BINARY_DIR}/TubeTKLauncher.bat @ONLY )

  find_program(CMD_EXECUTABLE "cmd")
  if( NOT CMD_EXECUTABLE )
    message(FATAL_ERROR
      "Could not find 'cmd' executable required to run test using the launcher")
  endif()

  set(TubeTK_LAUNCHER ${TubeTK_BINARY_DIR}/TubeTKLauncher.bat)
elseif( UNIX AND TubeTK_BUILD_SLICER_MODULES )
  # Apple OS X, Linux, or UNIX.
  set(_launcher_platform "unix")
  configure_file( ${TubeTK_SOURCE_DIR}/CMake/TubeTKLauncher.sh.in
    ${TubeTK_BINARY_DIR}/TubeTKLauncher.sh @ONLY )

  find_program(SH_EXECUTABLE "sh")
  if( NOT SH_EXECUTABLE )
    message(FATAL_ERROR
      "Could not find 'sh' executable required to run test using the launcher")
  endif()
  set(TubeTK_LAUNCHER ${SH_EXECUTABLE} ${TubeTK_BINARY_DIR}/TubeTKLauncher.sh)
elseif( NOT UNIX )
  message(FATAL_ERROR "Configuring Launcher script - failed [unknown platform]")
endif()
message(STATUS "Configuring Launcher script - ok [${_launcher_platform}]")

# Setup to conform to 3D Slicer paths and define plugin copying macros.
if( TubeTK_BUILD_APPLICATIONS AND NOT TubeTK_BUILD_SLICER_MODULES )
  set( TubeTK_SLICER_SOURCE_DIR "${TubeTK_BINARY_DIR}/Slicer" )
  set( TubeTK_SLICER_BINARY_DIR "${TubeTK_BINARY_DIR}/Slicer-build" )
  make_directory( ${TubeTK_SLICER_SOURCE_DIR} )
  make_directory( ${TubeTK_SLICER_BINARY_DIR} )

  include( TubeTKSlicer4MacroGetSource )

  TubeTKSlicer4MacroGetSource( Resources )
  TubeTKSlicer4MacroGetSource( Base/CLI )
  TubeTKSlicer4MacroGetSource( Libs/ITKFactoryRegistration )

  # Build SlicerBaseCLI
  add_subdirectory( ${TubeTK_SLICER_SOURCE_DIR}/Base/CLI
    ${TubeTK_SLICER_BINARY_DIR}/Base/CLI )
  set( Slicer_BaseCLI_INCLUDE_DIRS
    ${SlicerBaseCLI_SOURCE_DIR}
    ${SlicerBaseCLI_BINARY_DIR} )
  set( SUPPORT_2D_IMAGES on )

  # Build ITKFactorRegistration
  set( CXX_TEST_PATH ${TubeTK_BINARY_DIR}/${Slicer4_INSTALL_BIN_DIR} )
  set( LAUNCH_EXE "" )

  set( ITKFactoryRegistration_INSTALL_BIN_DIR ${TubeTK_INSTALL_BIN_DIR} )
  set( ITKFactoryRegistration_INSTALL_LIB_DIR ${TubeTK_INSTALL_LIB_DIR} )
  add_subdirectory(
    ${TubeTK_SLICER_SOURCE_DIR}/Libs/ITKFactoryRegistration
    ${TubeTK_SLICER_BINARY_DIR}/Libs/ITKFactoryRegistration )
  set( ITKFactoryRegistration_INCLUDE_DIRS
    ${ITKFactoryRegistration_SOURCE_DIR}
    ${ITKFactoryRegistration_BINARY_DIR} )

  # Config SlicerExecutionModel
  set( _sem_extra_executable_target_libraries ITKFactoryRegistration )
  if( TubeTK_USE_VTK )
    list( APPEND _sem_extra_executable_target_libraries SlicerBaseCLI )
  endif( TubeTK_USE_VTK )

  set( SlicerExecutionModel_EXTRA_EXECUTABLE_TARGET_LIBRARIES
    ${_sem_extra_executable_target_libraries}
    CACHE INTERNAL "3D Slicer execution model extra libraries." FORCE )

  set( _sem_extra_include_directories ${ITKFactoryRegistration_INCLUDE_DIRS} )
  if( TubeTK_USE_VTK )
    list( APPEND _sem_extra_include_directories ${Slicer_BaseCLI_INCLUDE_DIRS})
  endif( TubeTK_USE_VTK )

  set(SlicerExecutionModel_EXTRA_INCLUDE_DIRECTORIES
    ${_sem_extra_include_directories}
    CACHE INTERNAL "SlicerExecutionModel extra includes" FORCE )

  set(SlicerExecutionModel_CLI_LIBRARY_WRAPPER_CXX
    ${TubeTK_SLICER_BINARY_DIR}/Base/CLI/SEMCommandLineLibraryWrapper.cxx
    CACHE INTERNAL "SlicerExecutionModel extra includes" FORCE
    )
  configure_file(
    ${TubeTK_SLICER_SOURCE_DIR}/Base/CLI/SEMCommandLineLibraryWrapper.cxx.in
    ${SlicerExecutionModel_CLI_LIBRARY_WRAPPER_CXX}
    )

endif( TubeTK_BUILD_APPLICATIONS AND NOT TubeTK_BUILD_SLICER_MODULES )

include( TubeTKMacroInstallPlugins )

# Setup Python VirtualEnv for testing.
if( BUILD_TESTING AND TubeTK_USE_PYTHON AND NOT TubeTK_BUILD_SLICER_MODULES )
  include( TubeTKVirtualEnvSetup )
endif( BUILD_TESTING AND TubeTK_USE_PYTHON AND NOT TubeTK_BUILD_SLICER_MODULES )

# Test TubeTK IPython examples.
if( TubeTK_USE_NOTEBOOKS )
  add_subdirectory( Examples )
endif( TubeTK_USE_NOTEBOOKS )

add_subdirectory( Utilities )
add_subdirectory( Base )
if( TubeTK_BUILD_APPLICATIONS )
  add_subdirectory( Applications )
  if( TubeTK_BUILD_SLICER_MODULES )
    add_subdirectory( SlicerModules )
  endif( TubeTK_BUILD_SLICER_MODULES )
endif( TubeTK_BUILD_APPLICATIONS )

# Create the directory where CTest unit tests store temporary results.
make_directory( ${TubeTK_BINARY_DIR}/Temporary )

# Configure variables to export.
set( TubeTK_INCLUDE_DIRS
  ${TubeTK_SOURCE_DIR}/Base/CLI
  ${TubeTK_SOURCE_DIR}/Base/Common
  ${TubeTK_SOURCE_DIR}/Base/Filtering
  ${TubeTK_SOURCE_DIR}/Base/IO
  ${TubeTK_SOURCE_DIR}/Base/Numerics
  ${TubeTK_SOURCE_DIR}/Base/ObjectDocuments
  ${TubeTK_SOURCE_DIR}/Base/Registration
  ${TubeTK_SOURCE_DIR}/Base/Segmentation
  ${TubeTK_SOURCE_DIR}/Base/USTK )

set( TubeTK_LIBRARY_DIRS
  ${TubeTK_EXECUTABLE_DIRS} )

# Configure TubeTKTargets.cmake.
get_property( TubeTK_TARGETS GLOBAL PROPERTY TubeTK_TARGETS )
export( TARGETS ${TubeTK_TARGETS} FILE
  ${TubeTK_CONFIG_BINARY_DIR}/TubeTKTargets.cmake )

set( TubeTK_TARGETS_FILE
  ${TubeTK_CONFIG_BINARY_DIR}/TubeTKTargets.cmake )

set( TubeTK_USE_FILE
  ${TubeTK_CONFIG_BINARY_DIR}/UseTubeTK.cmake )

set(TubeTK_CONFIG_CODE
  "set(ParameterSerializer_DIR \"${ParameterSerializer_DIR}\")")

# Configure TubeTKConfig.cmake.
# The variables set above are for building against a TubeTK build tree.
# A different TubeTKConfig.cmake will need to be generated in order to build
# against a TubeTK installation.
configure_file( ${TubeTK_SOURCE_DIR}/CMake/TubeTKConfig.cmake.in
  ${TubeTK_CONFIG_BINARY_DIR}/TubeTKConfig.cmake @ONLY )

# Configure UseTubeTK.cmake.
configure_file( ${TubeTK_SOURCE_DIR}/CMake/UseTubeTK.cmake.in
  ${TubeTK_CONFIG_BINARY_DIR}/UseTubeTK.cmake @ONLY )

# Configure CPack packaging.
if( NOT TubeTK_BUILD_SLICER_MODULES )
  include( TubeTKCPackConfig )
 else( NOT TubeTK_BUILD_SLICER_MODULES )
  include( ${Slicer_EXTENSION_CPACK} )
endif( NOT TubeTK_BUILD_SLICER_MODULES )
