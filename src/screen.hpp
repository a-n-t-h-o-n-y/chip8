#ifndef SCREEN_HPP
#define SCREEN_HPP
#include <esc/io.hpp>
#include <esc/sequence.hpp>

#include "esc/sequence.hpp"
#include "instructions.hpp"
#include "types.hpp"
#include "state.hpp"

namespace chip8 {
inline auto is_graphics_instruction(Instruction_t instruction) -> bool
{
  return opcode(instruction) == 0xD || instruction == 0x00E0;
}

// #define BRAILLE

inline auto update_graphics(State const& state) -> void
{
  esc::write(esc::escape(esc::Cursor_position{0, 0}));
  auto const& buffer = state.screen_buffer;
#ifdef BRAILLE
  for (auto y = 0; y < 8; ++y) {
    for (auto x = 0; x < 32; ++x) {
      auto const y_off = y * 4;
      auto const x_off = x * 2;
      auto base        = U'⠀';
      if (buffer[y_off + 0][x_off + 0]) {
        base |= U'⠁';
      }
      if (buffer[y_off + 1][x_off + 0]) {
        base |= U'⠂';
      }
      if (buffer[y_off + 2][x_off + 0]) {
        base |= U'⠄';
      }
      if (buffer[y_off + 3][x_off + 0]) {
        base |= U'⡀';
      }
      if (buffer[y_off + 0][x_off + 1]) {
        base |= U'⠈';
      }
      if (buffer[y_off + 1][x_off + 1]) {
        base |= U'⠐';
      }
      if (buffer[y_off + 2][x_off + 1]) {
        base |= U'⠠';
      }
      if (buffer[y_off + 3][x_off + 1]) {
        base |= U'⢀';
      }
      esc::write(base);
    }
    esc::write('\n');
  }

#else
  for (auto y = 0; y < 16; ++y) {
    for (auto x = 0; x < 64; ++x) {
      auto block = U' ';
      if (buffer[y * 2][x] && buffer[(y * 2) + 1][x]) {
        block = U'█';
      }
      else if (buffer[y * 2][x]) {
        block = U'▀';
      }
      else if (buffer[(y * 2) + 1][x]) {
        block = U'▄';
      }
      esc::write(block);
    }
    esc::write('\n');
  }

#endif
  esc::flush();
}

}  // namespace chip8
#endif  // SCREEN_HPP
