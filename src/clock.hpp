#ifndef CLOCK_HPP
#define CLOCK_HPP
#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>

#include "types.hpp"

namespace chip8 {

inline auto emulated_clock(Instruction_t instruction)
  -> std::chrono::microseconds
{
  // TODO
  // https://jackson-s.me/2019/07/13/Chip-8-Instruction-Scheduling-and-Frequency.html
  return std::chrono::microseconds{1000000 / 500};
}

/// If clock_hz is nullopt, default to emulated per opcode averages.
inline auto make_clock_fn(std::optional<std::uint16_t> clock_hz)
  -> std::function<std::chrono::microseconds(Instruction_t)>
{
  if (!clock_hz.has_value()) {
    return emulated_clock;
  }
  return [hz = *clock_hz](Instruction_t) {
    return std::chrono::microseconds{1000000 / hz};
  };
}

}  // namespace chip8
#endif  // CLOCK_HPP
