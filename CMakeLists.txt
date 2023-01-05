cmake_minimum_required(VERSION 3.24)
project(chip8)

# Add the source files for the interpreter
add_executable(chip8
    src/main.cpp
)

# Set the C++ standard to C++20 for the interpreter target
target_compile_features(chip8 PRIVATE cxx_std_20)

# Link the interpreter with any necessary libraries
target_link_libraries(chip8 PRIVATE
    #SDL2
    #SDL2_ttf
)

