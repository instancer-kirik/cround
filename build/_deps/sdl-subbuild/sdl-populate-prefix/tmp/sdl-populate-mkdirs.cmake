# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/kirik/Code/Clay/cround/build/_deps/sdl-src")
  file(MAKE_DIRECTORY "/home/kirik/Code/Clay/cround/build/_deps/sdl-src")
endif()
file(MAKE_DIRECTORY
  "/home/kirik/Code/Clay/cround/build/_deps/sdl-build"
  "/home/kirik/Code/Clay/cround/build/_deps/sdl-subbuild/sdl-populate-prefix"
  "/home/kirik/Code/Clay/cround/build/_deps/sdl-subbuild/sdl-populate-prefix/tmp"
  "/home/kirik/Code/Clay/cround/build/_deps/sdl-subbuild/sdl-populate-prefix/src/sdl-populate-stamp"
  "/home/kirik/Code/Clay/cround/build/_deps/sdl-subbuild/sdl-populate-prefix/src"
  "/home/kirik/Code/Clay/cround/build/_deps/sdl-subbuild/sdl-populate-prefix/src/sdl-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/kirik/Code/Clay/cround/build/_deps/sdl-subbuild/sdl-populate-prefix/src/sdl-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/kirik/Code/Clay/cround/build/_deps/sdl-subbuild/sdl-populate-prefix/src/sdl-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
