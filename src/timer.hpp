#ifndef TIMER_HPP
#define TIMER_HPP
#include <algorithm>
#include <chrono>
#include <cstdint>

#include "types.hpp"

namespace chip8 {

inline auto update_timer(Timer_register& reg) -> void
{
  if (reg.value == 0) {
    return;
  }
  auto const elapsed = Clock_t::now() - reg.previous_update;
  auto const ticks   = std::uint8_t(elapsed / reg.rate);
  reg.value -= std::min(ticks, reg.value);
  if (ticks > 0) {
    reg.previous_update = Clock_t::now() - (elapsed % reg.rate);
  }
}

}  // namespace chip8
#endif  // TIMER_HPP
