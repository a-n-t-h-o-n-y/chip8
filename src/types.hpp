#ifndef TYPES_HPP
#define TYPES_HPP
#include <chrono>
#include <cstdint>

namespace chip8 {

using Clock_t       = std::chrono::high_resolution_clock;
using Instruction_t = std::uint16_t;
using Address_t     = std::uint16_t;

struct Timer_register {
  std::chrono::time_point<Clock_t> previous_update;
  std::uint8_t value{0};
  std::chrono::microseconds rate;
};

}  // namespace chip8
#endif  // TYPES_HPP
