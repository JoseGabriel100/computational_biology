# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = /home/jose/Desktop/sbm/traffic-simulation

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jose/Desktop/sbm/traffic-simulation/build

# Include any dependencies generated for this target.
include CMakeFiles/TrafficSimulation.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/TrafficSimulation.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/TrafficSimulation.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TrafficSimulation.dir/flags.make

CMakeFiles/TrafficSimulation.dir/main.cpp.o: CMakeFiles/TrafficSimulation.dir/flags.make
CMakeFiles/TrafficSimulation.dir/main.cpp.o: /home/jose/Desktop/sbm/traffic-simulation/main.cpp
CMakeFiles/TrafficSimulation.dir/main.cpp.o: CMakeFiles/TrafficSimulation.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/jose/Desktop/sbm/traffic-simulation/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TrafficSimulation.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/TrafficSimulation.dir/main.cpp.o -MF CMakeFiles/TrafficSimulation.dir/main.cpp.o.d -o CMakeFiles/TrafficSimulation.dir/main.cpp.o -c /home/jose/Desktop/sbm/traffic-simulation/main.cpp

CMakeFiles/TrafficSimulation.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/TrafficSimulation.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jose/Desktop/sbm/traffic-simulation/main.cpp > CMakeFiles/TrafficSimulation.dir/main.cpp.i

CMakeFiles/TrafficSimulation.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/TrafficSimulation.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jose/Desktop/sbm/traffic-simulation/main.cpp -o CMakeFiles/TrafficSimulation.dir/main.cpp.s

# Object files for target TrafficSimulation
TrafficSimulation_OBJECTS = \
"CMakeFiles/TrafficSimulation.dir/main.cpp.o"

# External object files for target TrafficSimulation
TrafficSimulation_EXTERNAL_OBJECTS =

TrafficSimulation: CMakeFiles/TrafficSimulation.dir/main.cpp.o
TrafficSimulation: CMakeFiles/TrafficSimulation.dir/build.make
TrafficSimulation: /usr/lib/x86_64-linux-gnu/libtcl.so
TrafficSimulation: /usr/lib/x86_64-linux-gnu/libtk.so
TrafficSimulation: CMakeFiles/TrafficSimulation.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/jose/Desktop/sbm/traffic-simulation/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable TrafficSimulation"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TrafficSimulation.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TrafficSimulation.dir/build: TrafficSimulation
.PHONY : CMakeFiles/TrafficSimulation.dir/build

CMakeFiles/TrafficSimulation.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TrafficSimulation.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TrafficSimulation.dir/clean

CMakeFiles/TrafficSimulation.dir/depend:
	cd /home/jose/Desktop/sbm/traffic-simulation/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jose/Desktop/sbm/traffic-simulation /home/jose/Desktop/sbm/traffic-simulation /home/jose/Desktop/sbm/traffic-simulation/build /home/jose/Desktop/sbm/traffic-simulation/build /home/jose/Desktop/sbm/traffic-simulation/build/CMakeFiles/TrafficSimulation.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/TrafficSimulation.dir/depend

