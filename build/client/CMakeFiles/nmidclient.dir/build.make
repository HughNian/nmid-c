# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/niansong/C/libnmid/client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/niansong/C/libnmid/build/client

# Include any dependencies generated for this target.
include CMakeFiles/nmidclient.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/nmidclient.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/nmidclient.dir/flags.make

CMakeFiles/nmidclient.dir/client.o: CMakeFiles/nmidclient.dir/flags.make
CMakeFiles/nmidclient.dir/client.o: /home/niansong/C/libnmid/client/client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/niansong/C/libnmid/build/client/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/nmidclient.dir/client.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/nmidclient.dir/client.o   -c /home/niansong/C/libnmid/client/client.c

CMakeFiles/nmidclient.dir/client.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/nmidclient.dir/client.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/niansong/C/libnmid/client/client.c > CMakeFiles/nmidclient.dir/client.i

CMakeFiles/nmidclient.dir/client.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/nmidclient.dir/client.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/niansong/C/libnmid/client/client.c -o CMakeFiles/nmidclient.dir/client.s

CMakeFiles/nmidclient.dir/hash.o: CMakeFiles/nmidclient.dir/flags.make
CMakeFiles/nmidclient.dir/hash.o: /home/niansong/C/libnmid/client/hash.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/niansong/C/libnmid/build/client/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/nmidclient.dir/hash.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/nmidclient.dir/hash.o   -c /home/niansong/C/libnmid/client/hash.c

CMakeFiles/nmidclient.dir/hash.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/nmidclient.dir/hash.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/niansong/C/libnmid/client/hash.c > CMakeFiles/nmidclient.dir/hash.i

CMakeFiles/nmidclient.dir/hash.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/nmidclient.dir/hash.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/niansong/C/libnmid/client/hash.c -o CMakeFiles/nmidclient.dir/hash.s

CMakeFiles/nmidclient.dir/rw.o: CMakeFiles/nmidclient.dir/flags.make
CMakeFiles/nmidclient.dir/rw.o: /home/niansong/C/libnmid/client/rw.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/niansong/C/libnmid/build/client/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/nmidclient.dir/rw.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/nmidclient.dir/rw.o   -c /home/niansong/C/libnmid/client/rw.c

CMakeFiles/nmidclient.dir/rw.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/nmidclient.dir/rw.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/niansong/C/libnmid/client/rw.c > CMakeFiles/nmidclient.dir/rw.i

CMakeFiles/nmidclient.dir/rw.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/nmidclient.dir/rw.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/niansong/C/libnmid/client/rw.c -o CMakeFiles/nmidclient.dir/rw.s

# Object files for target nmidclient
nmidclient_OBJECTS = \
"CMakeFiles/nmidclient.dir/client.o" \
"CMakeFiles/nmidclient.dir/hash.o" \
"CMakeFiles/nmidclient.dir/rw.o"

# External object files for target nmidclient
nmidclient_EXTERNAL_OBJECTS =

libnmidclient.so.1.2: CMakeFiles/nmidclient.dir/client.o
libnmidclient.so.1.2: CMakeFiles/nmidclient.dir/hash.o
libnmidclient.so.1.2: CMakeFiles/nmidclient.dir/rw.o
libnmidclient.so.1.2: CMakeFiles/nmidclient.dir/build.make
libnmidclient.so.1.2: CMakeFiles/nmidclient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/niansong/C/libnmid/build/client/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C shared library libnmidclient.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/nmidclient.dir/link.txt --verbose=$(VERBOSE)
	$(CMAKE_COMMAND) -E cmake_symlink_library libnmidclient.so.1.2 libnmidclient.so.1 libnmidclient.so

libnmidclient.so.1: libnmidclient.so.1.2
	@$(CMAKE_COMMAND) -E touch_nocreate libnmidclient.so.1

libnmidclient.so: libnmidclient.so.1.2
	@$(CMAKE_COMMAND) -E touch_nocreate libnmidclient.so

# Rule to build all files generated by this target.
CMakeFiles/nmidclient.dir/build: libnmidclient.so

.PHONY : CMakeFiles/nmidclient.dir/build

CMakeFiles/nmidclient.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/nmidclient.dir/cmake_clean.cmake
.PHONY : CMakeFiles/nmidclient.dir/clean

CMakeFiles/nmidclient.dir/depend:
	cd /home/niansong/C/libnmid/build/client && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/niansong/C/libnmid/client /home/niansong/C/libnmid/client /home/niansong/C/libnmid/build/client /home/niansong/C/libnmid/build/client /home/niansong/C/libnmid/build/client/CMakeFiles/nmidclient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/nmidclient.dir/depend

