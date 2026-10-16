# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-src")
  file(MAKE_DIRECTORY "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-src")
endif()
file(MAKE_DIRECTORY
  "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-build"
  "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-subbuild/tinycbor-populate-prefix"
  "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-subbuild/tinycbor-populate-prefix/tmp"
  "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-subbuild/tinycbor-populate-prefix/src/tinycbor-populate-stamp"
  "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-subbuild/tinycbor-populate-prefix/src"
  "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-subbuild/tinycbor-populate-prefix/src/tinycbor-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-subbuild/tinycbor-populate-prefix/src/tinycbor-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/nordic/knx_workshop/solution/sensor/_build/sensor/_deps/tinycbor-subbuild/tinycbor-populate-prefix/src/tinycbor-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
