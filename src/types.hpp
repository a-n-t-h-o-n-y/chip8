#ifndef TYPES_HPP
#define TYPES_HPP
#include <array>
#include <chrono>
#include <cstdint>
#include <optional>

#include "constants.hpp"

namespace chip8 {

using Clock_t       = std::chrono::high_resolution_clock;
using Instruction_t = std::uint16_t;
using Address_t     = std::uint16_t;

struct Timer_register {
  std::chrono::time_point<Clock_t> previous_update;
  std::uint8_t value{0};
  std::chrono::microseconds rate;
};

struct State {
  std::array<std::uint8_t, 16> general_purpose_registers{};
  std::uint16_t index_register{0};
  Address_t program_counter{INSTRUCTION_OFFSET};
  std::array<Address_t, 16> instruction_stack{};
  std::uint8_t stack_pointer{0};
  Timer_register delay_timer_register;
  Timer_register sound_timer_register;
  std::array<std::uint8_t, MEMORY_AMOUNT> memory{};
  std::optional<std::uint8_t> pressed_key{std::nullopt};
  std::array<std::array<bool, 64>, 32> screen_buffer;
};

}  // namespace chip8
#endif  // TYPES_HPP
