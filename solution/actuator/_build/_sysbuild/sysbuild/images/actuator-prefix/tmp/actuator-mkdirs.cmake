# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/nordic/knx_workshop/solution/actuator")
  file(MAKE_DIRECTORY "C:/nordic/knx_workshop/solution/actuator")
endif()
file(MAKE_DIRECTORY
  "C:/nordic/knx_workshop/solution/actuator/_build/actuator"
  "C:/nordic/knx_workshop/solution/actuator/_build/_sysbuild/sysbuild/images/actuator-prefix"
  "C:/nordic/knx_workshop/solution/actuator/_build/_sysbuild/sysbuild/images/actuator-prefix/tmp"
  "C:/nordic/knx_workshop/solution/actuator/_build/_sysbuild/sysbuild/images/actuator-prefix/src/actuator-stamp"
  "C:/nordic/knx_workshop/solution/actuator/_build/_sysbuild/sysbuild/images/actuator-prefix/src"
  "C:/nordic/knx_workshop/solution/actuator/_build/_sysbuild/sysbuild/images/actuator-prefix/src/actuator-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/nordic/knx_workshop/solution/actuator/_build/_sysbuild/sysbuild/images/actuator-prefix/src/actuator-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/nordic/knx_workshop/solution/actuator/_build/_sysbuild/sysbuild/images/actuator-prefix/src/actuator-stamp${cfgdir}") # cfgdir has leading slash
endif()
