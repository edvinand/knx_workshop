# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/nordic/knx_workshop/solution/sensor")
  file(MAKE_DIRECTORY "C:/nordic/knx_workshop/solution/sensor")
endif()
file(MAKE_DIRECTORY
  "C:/nordic/knx_workshop/solution/sensor/_build/sensor"
  "C:/nordic/knx_workshop/solution/sensor/_build/_sysbuild/sysbuild/images/sensor-prefix"
  "C:/nordic/knx_workshop/solution/sensor/_build/_sysbuild/sysbuild/images/sensor-prefix/tmp"
  "C:/nordic/knx_workshop/solution/sensor/_build/_sysbuild/sysbuild/images/sensor-prefix/src/sensor-stamp"
  "C:/nordic/knx_workshop/solution/sensor/_build/_sysbuild/sysbuild/images/sensor-prefix/src"
  "C:/nordic/knx_workshop/solution/sensor/_build/_sysbuild/sysbuild/images/sensor-prefix/src/sensor-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/nordic/knx_workshop/solution/sensor/_build/_sysbuild/sysbuild/images/sensor-prefix/src/sensor-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/nordic/knx_workshop/solution/sensor/_build/_sysbuild/sysbuild/images/sensor-prefix/src/sensor-stamp${cfgdir}") # cfgdir has leading slash
endif()
