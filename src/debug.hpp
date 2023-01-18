#ifndef CHIP8_DEBUG_HPP
#define CHIP8_DEBUG_HPP
#include <ios>
#include <ostream>

#include "state.hpp"
#include "types.hpp"

namespace chip8 {

inline auto write_stack(std::ostream& os, std::array<Address_t, 16> stack)
  -> void
{
  os << "Instruction Stack:\n";
  for (auto i = 0; i < stack.size(); ++i) {
    os << "\t0x" << i << " 0x" << stack[i] << '\n';
  }
}

inline auto write_registers(std::ostream& os,
                            std::array<std::uint8_t, 16> registers) -> void
{
  os << "Registers:\n";
  for (auto i = 0; i < registers.size(); ++i) {
    os << "\tV" << i << " 0x" << (int)registers[i] << '\n';
  }
}

inline auto write_state(std::ostream& os, State const& state) -> std::ostream&
{
  os << std::hex;
  os << "Next Instruction: "
     << "0x" << (int)state.memory[state.program_counter] << '\n';
  write_registers(os, state.general_purpose_registers);
  os << "Index Register:   "
     << "0x" << state.index_register << '\n';
  os << "Program Counter:  "
     << "0x" << state.program_counter << '\n';
  os << "Delay Timer:      "
     << "0x" << (int)state.delay_timer_register.value << '\n';
  os << "Sound Timer:      "
     << "0x" << (int)state.delay_timer_register.value << '\n';
  os << "Stack Pointer:    "
     << "0x" << (int)state.stack_pointer << '\n';
  write_stack(os, state.instruction_stack);

  os.flush();
  return os;
}

inline auto print_memory(std::ostream& os,
                         std::array<std::uint8_t, MEMORY_AMOUNT> const& memory)
  -> std::ostream&
{
  os << std::hex;
  for (char c : memory) {
    // if (c != 0) {
    os << (0xFF & c);
    // }
  }
  os << std::endl;
  return os;
}

inline auto print_screen_state(std::ostream& os, State const& state) -> void
{
  for (auto const& row : state.screen_buffer) {
    for (bool const pixel : row) {
      os << (pixel ? 'X' : '_');
    }
    os << std::endl;
  }
}

}  // namespace chip8

#endif  // CHIP8_DEBUG_HPP
