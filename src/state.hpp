#ifndef CHIP8_STATE_HPP
#define CHIP8_STATE_HPP
#include <array>
#include <cstdint>
#include <optional>

#include "constants.hpp"
#include "keyboard.hpp"
#include "types.hpp"

namespace chip8 {

struct State {
  std::array<std::uint8_t, 16> general_purpose_registers{};
  std::uint16_t index_register{0};
  Address_t program_counter{INSTRUCTION_OFFSET};
  std::array<Address_t, 16> instruction_stack{};
  std::uint8_t stack_pointer{0};
  Timer_register delay_timer_register;
  Timer_register sound_timer_register;
  std::array<std::uint8_t, MEMORY_AMOUNT> memory{};
  Keyboard<75> keyboard;
  std::array<std::array<bool, 64>, 32> screen_buffer;
};

}  // namespace chip8
#endif  // CHIP8_STATE_HPP
