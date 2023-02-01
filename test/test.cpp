#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "../src/debug.hpp"
#include "../src/instructions.hpp"
#include "../src/state.hpp"
#include "../src/types.hpp"

using namespace esc;

template <typename T>
auto test_equal(T a, T b) -> void
{
  if (!(a == b)) {
    std::cerr << std::hex << "ERROR\n0x" << a << " == "
              << "0x" << b << '\n';
  }
}

template <typename T>
auto test_not_equal(T a, T b) -> void
{
  if (!(a != b)) {
    std::cerr << std::hex << "ERROR\n0x" << a << " != "
              << "0x" << b << '\n';
  }
}

// TODO
//  8xy3 - XOR Vx, Vy

// 8xy4 - ADD Vx, Vy
auto test01() -> void
{
  // Set Vx = Vx + Vy, set VF = carry.
  //  The values of Vx and Vy are added together. If the result is greater than
  //  8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits
  //  of the result are kept, and stored in Vx.

  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x0]   = 0x5;
    reg[0x1]   = 0x4;
    process_instruction(state, 0x8014);
    test_equal((int)reg[0x0], 0x9);
    test_equal((int)reg[0xF], 0x0);
  }
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x0]   = 0xFF;
    reg[0x1]   = 0xFF;
    process_instruction(state, 0x8014);
    test_equal((int)reg[0x0], 0b11111110);
    test_equal((int)reg[0xF], 0x1);
  }
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x0]   = 0xF0;
    reg[0x1]   = 0x0F;
    process_instruction(state, 0x8014);
    test_equal((int)reg[0x0], 0xFF);
    test_equal((int)reg[0xF], 0x0);
  }
}

// 8xy5 - SUB Vx, Vy
auto test02() -> void
{
  // Set Vx = Vx - Vy, set VF = NOT borrow.
  //  If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from
  //  Vx, and the results stored in Vx.

  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 0xF0;
    reg[0x3]   = 0x0F;
    process_instruction(state, 0x8235);
    test_equal((int)reg[0x2], 0xE1);
    test_equal((int)reg[0xF], 0x1);
  }
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 0x05;
    reg[0x3]   = 0x05;
    process_instruction(state, 0x8235);
    test_equal((int)reg[0x2], 0x0);
    test_equal((int)reg[0xF], 0x0);
  }
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 0x15;
    reg[0x3]   = 0x7A;
    process_instruction(state, 0x8235);
    test_equal((int)reg[0x2], 0x9b);
    test_equal((int)reg[0xF], 0x0);
  }
}

// 8xy6 - SHR Vx {, Vy}
auto test03() -> void
{
  // Set Vx = Vx SHR 1.
  //  If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
  //  Then Vx is divided by 2.

  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 100;
    process_instruction(state, 0x8236);
    test_equal((int)reg[0x2], 50);
    test_equal((int)reg[0xF], 0);
  }
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 25;
    process_instruction(state, 0x8236);
    test_equal((int)reg[0x2], 12);
    test_equal((int)reg[0xF], 1);
  }
}

//  8xy7 - SUBN Vx, Vy
auto test04() -> void
{
  // Set Vx = Vy - Vx, set VF = NOT borrow.
  //  If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from
  //  Vy, and the results stored in Vx.
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 0xF0;
    reg[0x3]   = 0x0F;
    process_instruction(state, 0x8237);
    test_equal((int)reg[0x2], 0x1F);
    test_equal((int)reg[0xF], 0x0);
  }
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 0x05;
    reg[0x3]   = 0x05;
    process_instruction(state, 0x8237);
    test_equal((int)reg[0x2], 0x0);
    test_equal((int)reg[0xF], 0x0);
  }
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 0x15;
    reg[0x3]   = 0x7A;
    process_instruction(state, 0x8237);
    test_equal((int)reg[0x2], 0x65);
    test_equal((int)reg[0xF], 0x1);
  }
}

// 8xyE - SHL Vx {, Vy}
auto test05() -> void
{
  // Set Vx = Vx SHL 1.
  //  If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to
  //  0. Then Vx is multiplied by 2.
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 50;
    process_instruction(state, 0x823E);
    test_equal((int)reg[0x2], 100);
    test_equal((int)reg[0xF], 0);
  }
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    reg[0x2]   = 141;
    process_instruction(state, 0x823E);
    test_equal((int)reg[0x2], 26);
    test_equal((int)reg[0xF], 1);
  }
}

// 9xy0 - SNE Vx, Vy
auto test06() -> void
{
  // Skip next instruction if Vx != Vy
  //  The values of Vx and Vy are compared, and if they are not equal, the
  //  program counter is increased by 2.
  {
    auto state            = State{};
    state.program_counter = 0x10;
    auto& reg             = state.general_purpose_registers;
    reg[0x6]              = 5;
    reg[0x4]              = 141;
    process_instruction(state, 0x9640);
    test_equal((int)state.program_counter, 0x12);
  }
  {
    auto state            = State{};
    state.program_counter = 0x10;
    auto& reg             = state.general_purpose_registers;
    reg[0x6]              = 141;
    reg[0x4]              = 141;
    process_instruction(state, 0x9640);
    test_equal((int)state.program_counter, 0x10);
  }
}

// Annn - LD I, addr
auto test07() -> void
{
  // Set I = nnn.
  //  The value of register I is set to nnn.
  {
    auto state = State{};
    process_instruction(state, 0xA000);
    test_equal((int)state.index_register, 0x000);
  }
  {
    auto state = State{};
    process_instruction(state, 0xA123);
    test_equal((int)state.index_register, 0x123);
  }
  {
    auto state = State{};
    process_instruction(state, 0xAFFF);
    test_equal((int)state.index_register, 0xFFF);
  }
}

// Bnnn - JP V0, addr
auto test08() -> void
{
  // Jump to location nnn + V0.
  //  The program counter is set to nnn plus the value of V0.
  {
    auto state            = State{};
    state.program_counter = 0x123;
    auto& reg             = state.general_purpose_registers;
    reg[0x0]              = 0x62;
    process_instruction(state, 0xB123);
    test_equal((int)state.program_counter, 0x185);
  }
  {
    auto state            = State{};
    state.program_counter = 0x123;
    auto& reg             = state.general_purpose_registers;
    reg[0x0]              = 0xFF;
    process_instruction(state, 0xBFFF);
    test_equal((int)state.program_counter, 0x10FE);
  }
}

// Cxkk - RND Vx, byte
auto test09() -> void
{
  // Set Vx = random byte AND kk.
  //  The interpreter generates a random number from 0 to 255, which is then
  //  ANDed with the value kk. The results are stored in Vx.
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    process_instruction(state, 0xC5FF);
    process_instruction(state, 0xC6FF);
    test_not_equal(reg[0x5], reg[0x6]);
  }
  {
    auto state = State{};
    auto& reg  = state.general_purpose_registers;
    process_instruction(state, 0xC500);
    test_equal((int)reg[0x5], 0x0);
  }
}

// Dxyn - DRW Vx, Vy, nibble
auto test10() -> void
{
  // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF =
  // collision.
  //  The interpreter reads n bytes from memory, starting at the address stored
  //  in I. These bytes are then displayed as sprites on screen at coordinates
  //  (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any
  //  pixels to be erased, VF is set to 1, otherwise it is set to 0. If the
  //  sprite is positioned so part of it is outside the coordinates of the
  //  display, it wraps around to the opposite side of the screen. See
  //  instruction 8xy3 for more information on XOR, and section 2.4, Display,
  //  for more information on the Chip-8 screen and sprites.
  {
    auto state = State{};

    constexpr auto sprite_bytes = 0x4;
    constexpr auto sprite_start = Address_t{0x123};
    constexpr auto sprite =
      std::array<std::uint8_t, sprite_bytes>{0xFF, 0x55, 0x82, 0x1D};
    for (auto i = 0; i < sprite_bytes; ++i) {
      state.memory[sprite_start + i] = sprite[i];
    }

    auto& reg            = state.general_purpose_registers;
    state.index_register = sprite_start;
    reg[0x5]             = 0x4;
    reg[0x6]             = 0x10;

    process_instruction(state, 0xD560 + sprite_bytes);

    for (auto i = 0; i < sprite_bytes; ++i) {
      // loop over 8 bits
      for (auto j = 0; j < 8; j++) {
        auto const screen_x = reg[0x5] + j;
        auto const screen_y = reg[0x6] + i;
        test_equal(state.screen_buffer[screen_y][screen_x],
                   (bool)(sprite[i] & (0x1 << (7 - j))));
      }
    }
    test_equal((int)reg[0xF], 0x0);

    // XOR and Collision
    process_instruction(state, 0xD560 + sprite_bytes);
    for (auto const& row : state.screen_buffer) {
      for (bool const pixel : row) {
        test_equal(pixel, false);
      }
    }
    test_equal((int)reg[0xF], 0x1);

    // Wraparound
    reg[0x5] = 0x3C;
    reg[0x6] = 0x23;
    reg[0xF] = 0x0;
    process_instruction(state, 0xD561);
    for (auto j = 0; j < 8; j++) {
      auto const screen_x = (reg[0x5] + j) % 64;
      auto const screen_y = reg[0x6] % 32;
      test_equal(state.screen_buffer[screen_y][screen_x], true);
    }
    test_equal((int)reg[0xF], 0x0);
  }
}

auto main() -> int
{
  test01();
  test02();
  test03();
  test04();
  test05();
  test06();
  test07();
  test08();
  test09();
  test10();

  return 0;
}
