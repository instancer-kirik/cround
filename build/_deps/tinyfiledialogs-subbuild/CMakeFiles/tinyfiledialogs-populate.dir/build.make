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
CMAKE_SOURCE_DIR = /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild

# Utility rule file for tinyfiledialogs-populate.

# Include any custom commands dependencies for this target.
include CMakeFiles/tinyfiledialogs-populate.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tinyfiledialogs-populate.dir/progress.make

CMakeFiles/tinyfiledialogs-populate: CMakeFiles/tinyfiledialogs-populate-complete

CMakeFiles/tinyfiledialogs-populate-complete: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-install
CMakeFiles/tinyfiledialogs-populate-complete: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-mkdir
CMakeFiles/tinyfiledialogs-populate-complete: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-download
CMakeFiles/tinyfiledialogs-populate-complete: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update
CMakeFiles/tinyfiledialogs-populate-complete: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-patch
CMakeFiles/tinyfiledialogs-populate-complete: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-configure
CMakeFiles/tinyfiledialogs-populate-complete: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-build
CMakeFiles/tinyfiledialogs-populate-complete: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-install
CMakeFiles/tinyfiledialogs-populate-complete: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-test
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Completed 'tinyfiledialogs-populate'"
	/usr/bin/cmake -E make_directory /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles
	/usr/bin/cmake -E touch /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles/tinyfiledialogs-populate-complete
	/usr/bin/cmake -E touch /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-done

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update:
.PHONY : tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-build: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-configure
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "No build step for 'tinyfiledialogs-populate'"
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-build && /usr/bin/cmake -E echo_append
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-build && /usr/bin/cmake -E touch /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-build

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-configure: tinyfiledialogs-populate-prefix/tmp/tinyfiledialogs-populate-cfgcmd.txt
tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-configure: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-patch
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "No configure step for 'tinyfiledialogs-populate'"
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-build && /usr/bin/cmake -E echo_append
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-build && /usr/bin/cmake -E touch /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-configure

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-download: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-gitinfo.txt
tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-download: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-mkdir
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Performing download step (git clone) for 'tinyfiledialogs-populate'"
	cd /home/kirik/Code/Clay/cround/build/_deps && /usr/bin/cmake -DCMAKE_MESSAGE_LOG_LEVEL=VERBOSE -P /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/tmp/tinyfiledialogs-populate-gitclone.cmake
	cd /home/kirik/Code/Clay/cround/build/_deps && /usr/bin/cmake -E touch /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-download

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-install: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-build
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "No install step for 'tinyfiledialogs-populate'"
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-build && /usr/bin/cmake -E echo_append
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-build && /usr/bin/cmake -E touch /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-install

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-mkdir:
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Creating directories for 'tinyfiledialogs-populate'"
	/usr/bin/cmake -Dcfgdir= -P /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/tmp/tinyfiledialogs-populate-mkdirs.cmake
	/usr/bin/cmake -E touch /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-mkdir

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-patch: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-patch-info.txt
tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-patch: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "No patch step for 'tinyfiledialogs-populate'"
	/usr/bin/cmake -E echo_append
	/usr/bin/cmake -E touch /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-patch

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update:
.PHONY : tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-test: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-install
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "No test step for 'tinyfiledialogs-populate'"
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-build && /usr/bin/cmake -E echo_append
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-build && /usr/bin/cmake -E touch /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-test

tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update: tinyfiledialogs-populate-prefix/tmp/tinyfiledialogs-populate-gitupdate.cmake
tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update-info.txt
tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-download
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Performing update step for 'tinyfiledialogs-populate'"
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-src && /usr/bin/cmake -Dcan_fetch=YES -DCMAKE_MESSAGE_LOG_LEVEL=VERBOSE -P /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/tinyfiledialogs-populate-prefix/tmp/tinyfiledialogs-populate-gitupdate.cmake

CMakeFiles/tinyfiledialogs-populate.dir/codegen:
.PHONY : CMakeFiles/tinyfiledialogs-populate.dir/codegen

tinyfiledialogs-populate: CMakeFiles/tinyfiledialogs-populate
tinyfiledialogs-populate: CMakeFiles/tinyfiledialogs-populate-complete
tinyfiledialogs-populate: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-build
tinyfiledialogs-populate: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-configure
tinyfiledialogs-populate: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-download
tinyfiledialogs-populate: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-install
tinyfiledialogs-populate: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-mkdir
tinyfiledialogs-populate: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-patch
tinyfiledialogs-populate: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-test
tinyfiledialogs-populate: tinyfiledialogs-populate-prefix/src/tinyfiledialogs-populate-stamp/tinyfiledialogs-populate-update
tinyfiledialogs-populate: CMakeFiles/tinyfiledialogs-populate.dir/build.make
.PHONY : tinyfiledialogs-populate

# Rule to build all files generated by this target.
CMakeFiles/tinyfiledialogs-populate.dir/build: tinyfiledialogs-populate
.PHONY : CMakeFiles/tinyfiledialogs-populate.dir/build

CMakeFiles/tinyfiledialogs-populate.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tinyfiledialogs-populate.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tinyfiledialogs-populate.dir/clean

CMakeFiles/tinyfiledialogs-populate.dir/depend:
	cd /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild /home/kirik/Code/Clay/cround/build/_deps/tinyfiledialogs-subbuild/CMakeFiles/tinyfiledialogs-populate.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/tinyfiledialogs-populate.dir/depend

