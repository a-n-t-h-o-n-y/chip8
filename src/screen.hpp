#ifndef SCREEN_HPP
#define SCREEN_HPP
#include <iostream>

#include "instructions.hpp"
#include "types.hpp"

namespace chip8 {
inline auto is_graphics_instruction(Instruction_t instruction) -> bool
{
  return opcode(instruction) == 0xD || instruction == 0x00E0;
}

inline auto update_graphics(State const& state) -> void
{
  // TODO
  // use the escape library to draw to the screen.
  std::cout << "Printing..." << std::endl;
}

}  // namespace chip8
#endif  // SCREEN_HPP
