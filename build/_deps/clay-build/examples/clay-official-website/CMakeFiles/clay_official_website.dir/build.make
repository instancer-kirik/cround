# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kirik/Code/Clay/cround

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kirik/Code/Clay/cround/build

# Include any dependencies generated for this target.
include _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/compiler_depend.make

# Include the progress variables for this target.
include _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/progress.make

# Include the compile flags for this target's objects.
include _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/flags.make

_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/codegen:
.PHONY : _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/codegen

_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/main.c.o: _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/flags.make
_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/main.c.o: _deps/clay-src/examples/clay-official-website/main.c
_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/main.c.o: _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/kirik/Code/Clay/cround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/main.c.o"
	cd /home/kirik/Code/Clay/cround/build/_deps/clay-build/examples/clay-official-website && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/main.c.o -MF CMakeFiles/clay_official_website.dir/main.c.o.d -o CMakeFiles/clay_official_website.dir/main.c.o -c /home/kirik/Code/Clay/cround/build/_deps/clay-src/examples/clay-official-website/main.c

_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/clay_official_website.dir/main.c.i"
	cd /home/kirik/Code/Clay/cround/build/_deps/clay-build/examples/clay-official-website && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/kirik/Code/Clay/cround/build/_deps/clay-src/examples/clay-official-website/main.c > CMakeFiles/clay_official_website.dir/main.c.i

_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/clay_official_website.dir/main.c.s"
	cd /home/kirik/Code/Clay/cround/build/_deps/clay-build/examples/clay-official-website && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/kirik/Code/Clay/cround/build/_deps/clay-src/examples/clay-official-website/main.c -o CMakeFiles/clay_official_website.dir/main.c.s

# Object files for target clay_official_website
clay_official_website_OBJECTS = \
"CMakeFiles/clay_official_website.dir/main.c.o"

# External object files for target clay_official_website
clay_official_website_EXTERNAL_OBJECTS =

_deps/clay-build/examples/clay-official-website/clay_official_website: _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/main.c.o
_deps/clay-build/examples/clay-official-website/clay_official_website: _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/build.make
_deps/clay-build/examples/clay-official-website/clay_official_website: _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/compiler_depend.ts
_deps/clay-build/examples/clay-official-website/clay_official_website: _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/kirik/Code/Clay/cround/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable clay_official_website"
	cd /home/kirik/Code/Clay/cround/build/_deps/clay-build/examples/clay-official-website && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/clay_official_website.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/build: _deps/clay-build/examples/clay-official-website/clay_official_website
.PHONY : _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/build

_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/clean:
	cd /home/kirik/Code/Clay/cround/build/_deps/clay-build/examples/clay-official-website && $(CMAKE_COMMAND) -P CMakeFiles/clay_official_website.dir/cmake_clean.cmake
.PHONY : _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/clean

_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/depend:
	cd /home/kirik/Code/Clay/cround/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kirik/Code/Clay/cround /home/kirik/Code/Clay/cround/build/_deps/clay-src/examples/clay-official-website /home/kirik/Code/Clay/cround/build /home/kirik/Code/Clay/cround/build/_deps/clay-build/examples/clay-official-website /home/kirik/Code/Clay/cround/build/_deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : _deps/clay-build/examples/clay-official-website/CMakeFiles/clay_official_website.dir/depend

