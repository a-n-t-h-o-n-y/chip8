#ifndef INITIALIZE_HPP
#define INITIALIZE_HPP
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "constants.hpp"
#include "state.hpp"
#include "types.hpp"

namespace chip8 {

inline auto digit_sprite_location(std::uint8_t digit) -> Address_t
{
  return (digit * 5u) + DIGIT_SPRITE_OFFSET;
}

inline auto initialize_digit_sprites(
  std::array<std::uint8_t, MEMORY_AMOUNT>& memory)
{
  constexpr auto digit_sprites = std::array<std::array<std::uint8_t, 5>, 16>{{
    {0b11110000, 0b10010000, 0b10010000, 0b10010000, 0b11110000},
    {0b00100000, 0b01100000, 0b00100000, 0b00100000, 0b01110000},
    {0b11110000, 0b00010000, 0b11110000, 0b10000000, 0b11110000},
    {0b11110000, 0b00010000, 0b11110000, 0b00010000, 0b11110000},
    {0b10010000, 0b10010000, 0b11110000, 0b00010000, 0b00010000},
    {0b11110000, 0b10000000, 0b11110000, 0b00010000, 0b11110000},
    {0b11110000, 0b10000000, 0b11110000, 0b10010000, 0b11110000},
    {0b11110000, 0b00010000, 0b00100000, 0b01000000, 0b01000000},
    {0b11110000, 0b10010000, 0b11110000, 0b10010000, 0b11110000},
    {0b11110000, 0b10010000, 0b11110000, 0b00010000, 0b11110000},
    {0b11110000, 0b10010000, 0b11110000, 0b10010000, 0b10010000},
    {0b11100000, 0b10010000, 0b11100000, 0b10010000, 0b11100000},
    {0b11110000, 0b10000000, 0b10000000, 0b10000000, 0b11110000},
    {0b11100000, 0b10010000, 0b10010000, 0b10010000, 0b11100000},
    {0b11110000, 0b10000000, 0b11110000, 0b10000000, 0b11110000},
    {0b11110000, 0b10000000, 0b11110000, 0b10000000, 0b10000000},
  }};
  // for each sprite
  for (auto i = Address_t{0x0}; i <= 0xF; ++i) {
    // for each row in the sprite
    auto const& sprite = digit_sprites[i];
    for (auto j = Address_t{0x0}; j < 0x5; ++j) {
      memory[digit_sprite_location(i) + j] = sprite[j];
    }
  }
}

inline auto initialize_screen(std::array<std::array<bool, 64>, 32>& screen)
  -> void
{
  using std::ranges::fill;
  for (auto& row : screen) {
    fill(row, false);
  }
}

inline auto initialize_state(std::vector<char> const& program) -> State
{
  using std::ranges::fill;
  if (program.size() > MEMORY_AMOUNT - INSTRUCTION_OFFSET) {
    auto ss = std::ostringstream{};
    ss << "Program size is too large (" << program.size()
       << "), cannot be greater than " << MEMORY_AMOUNT - INSTRUCTION_OFFSET;
    throw std::runtime_error{ss.str()};
  }
  auto state = State{};
  fill(state.general_purpose_registers, 0);
  fill(state.instruction_stack, Address_t{0});
  fill(state.memory, '\0');
  initialize_screen(state.screen_buffer);
  initialize_digit_sprites(state.memory);
  std::ranges::copy(program,
                    std::next(std::begin(state.memory), INSTRUCTION_OFFSET));

  state.delay_timer_register.previous_update = Clock_t::now();
  state.delay_timer_register.rate = std::chrono::microseconds{1000000 / 60};

  state.sound_timer_register.previous_update = Clock_t::now();
  state.sound_timer_register.rate = std::chrono::microseconds{1000000 / 60};

  return state;
}

inline auto load_program(std::string const& filepath) -> std::vector<char>
{
  auto input = std::ifstream{filepath, std::ios::binary};
  if (!input) {
    throw std::runtime_error("Error opening file");
  }
  auto program = std::vector<char>{};

  input.seekg(0, std::ios::end);
  program.resize(input.tellg());
  input.seekg(0, std::ios::beg);
  input.read(program.data(), program.size());

  return program;
}

}  // namespace chip8
#endif  // INITIALIZE_HPP
