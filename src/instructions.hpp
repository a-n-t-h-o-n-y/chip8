#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP
#include <algorithm>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "constants.hpp"
#include "types.hpp"

namespace {  // FIXME this is a header, anon namespace not right
using namespace chip8;

inline auto print_memory(std::array<char, MEMORY_AMOUNT> const& memory) -> void
{
  std::cout << std::hex;
  for (char c : memory) {
    if (c != 0) {
      std::cout << (0xFF & c);
    }
  }
  std::cout << std::endl;
}

inline auto print_registers(State const& state) -> void
{
  std::cout << std::hex;
  for (auto const reg : state.general_purpose_registers) {
    std::cout << (0xFF & reg) << ' ';
  }
  std::cout << std::endl;
}

inline auto opcode(Instruction_t instruction) -> std::uint8_t
{
  return instruction >> 12;
}

inline auto x(Instruction_t instruction) -> std::uint8_t
{
  return instruction & 0x0F00 >> 8;
}

inline auto y(Instruction_t instruction) -> std::uint8_t
{
  return instruction & 0x00F0 >> 4;
}

inline auto n(Instruction_t instruction) -> std::uint8_t
{
  return instruction & 0x000F;
}

inline auto nnn(Instruction_t instruction) -> std::uint16_t
{
  return instruction & 0x0FFF;
}

inline auto kk(Instruction_t instruction) -> std::uint8_t
{
  return instruction & 0x00FF;
}

inline auto clear_display(State& state) -> void
{
  for (auto& row : state.screen_buffer) {
    std::ranges::fill(row, false);
  }
}

inline auto subroutine_return(State& state) -> void
{
  state.program_counter = state.instruction_stack[state.stack_pointer];
  state.stack_pointer -= 1;
}

inline auto unknown_instruction_exception(Instruction_t instruction)
  -> std::runtime_error
{
  auto ss = std::stringstream{};
  ss << std::hex << instruction;
  return std::runtime_error{"Unknown Instruction: " + ss.str()};
}

inline auto jump_to_address(State& state, Instruction_t instruction) -> void
{
  state.program_counter = nnn(instruction);
}

inline auto call_subroutine(State& state, Instruction_t instruction) -> void
{
  state.stack_pointer += 1;
  state.instruction_stack[state.stack_pointer] = state.program_counter;
  state.program_counter                        = nnn(instruction);
}

/// Skip the next instruction if Vx == kk
inline auto skip_if_equal_rb(State& state, Instruction_t instruction) -> void
{
  if (state.general_purpose_registers[x(instruction)] == kk(instruction)) {
    state.program_counter += 2;
  }
}

/// Skip the next instruction if Vx != kk
inline auto skip_if_not_equal_rb(State& state, Instruction_t instruction)
  -> void
{
  if (state.general_purpose_registers[x(instruction)] != kk(instruction)) {
    state.program_counter += 2;
  }
}

/// Skip the next instruction if Vx == Vy
inline auto skip_if_equal_rr(State& state, Instruction_t instruction) -> void
{
  if (n(instruction) != 0) {
    throw unknown_instruction_exception(instruction);
  }
  if (state.general_purpose_registers[x(instruction)] ==
      state.general_purpose_registers[y(instruction)]) {
    state.program_counter += 2;
  }
}

/// Skip the next instruction if Vx != Vy
inline auto skip_if_not_equal_rr(State& state, Instruction_t instruction)
  -> void
{
  if (n(instruction) != 0) {
    throw unknown_instruction_exception(instruction);
  }
  if (state.general_purpose_registers[x(instruction)] !=
      state.general_purpose_registers[y(instruction)]) {
    state.program_counter += 2;
  }
}

/// Sets Vx to value kk.
inline auto set_register(State& state, Instruction_t instruction) -> void
{
  state.general_purpose_registers[x(instruction)] = kk(instruction);
}

/// Adds value kk to register Vx
inline auto add_register(State& state, Instruction_t instruction) -> void
{
  state.general_purpose_registers[x(instruction)] += kk(instruction);
}

inline auto load_y_to_x(State& state, Instruction_t instruction) -> void
{
  state.general_purpose_registers[x(instruction)] =
    state.general_purpose_registers[y(instruction)];
}

inline auto bitwise_or(State& state, Instruction_t instruction) -> void
{
  state.general_purpose_registers[x(instruction)] |=
    state.general_purpose_registers[y(instruction)];
}

inline auto bitwise_and(State& state, Instruction_t instruction) -> void
{
  state.general_purpose_registers[x(instruction)] &=
    state.general_purpose_registers[y(instruction)];
}

inline auto bitwise_xor(State& state, Instruction_t instruction) -> void
{
  state.general_purpose_registers[x(instruction)] ^=
    state.general_purpose_registers[y(instruction)];
}

inline auto add_with_carry(State& state, Instruction_t instruction) -> void
{
  state.general_purpose_registers[x(instruction)] +=
    state.general_purpose_registers[y(instruction)];
  if (x(instruction) > 0 &&
      (y(instruction) >
       std::numeric_limits<std::uint8_t>::max() - x(instruction))) {
    state.general_purpose_registers[0xFu] = 1;
  }
  else {
    state.general_purpose_registers[0xFu] = 0;
  }
}

inline auto subtract_with_not_borrow(State& state, Instruction_t instruction)
  -> void
{
  if (x(instruction) > y(instruction)) {
    state.general_purpose_registers[0xFu] = 1;
  }
  else {
    state.general_purpose_registers[0xFu] = 0;
  }
  state.general_purpose_registers[x(instruction)] -=
    state.general_purpose_registers[y(instruction)];
}

inline auto rsubtract_with_not_borrow(State& state, Instruction_t instruction)
  -> void
{
  if (y(instruction) > x(instruction)) {
    state.general_purpose_registers[0xFu] = 1;
  }
  else {
    state.general_purpose_registers[0xFu] = 0;
  }
  state.general_purpose_registers[x(instruction)] =
    state.general_purpose_registers[y(instruction)] -
    state.general_purpose_registers[x(instruction)];
}

inline auto shift_right(State& state, Instruction_t instruction) -> void
{
  state.general_purpose_registers[0xFu] =
    state.general_purpose_registers[x(instruction)] & 1u;
  state.general_purpose_registers[x(instruction)] /= 2u;
}

inline auto shift_left(State& state, Instruction_t instruction) -> void
{
  state.general_purpose_registers[0xFu] =
    state.general_purpose_registers[x(instruction)] & (1u << 7);
  state.general_purpose_registers[x(instruction)] *= 2u;
}

inline auto set_index_register(State& state, Instruction_t instruction) -> void
{
  state.index_register = nnn(instruction);
}

inline auto jump_to_nnn_plus_v0(State& state, Instruction_t instruction) -> void
{
  state.program_counter =
    nnn(instruction) + state.general_purpose_registers[0x0];
}

inline auto random_byte(State& state, Instruction_t instruction) -> void
{
  auto rng  = std::mt19937{std::random_device{}()};
  auto dist = std::uniform_int_distribution{0, 255};
  state.general_purpose_registers[x(instruction)] = dist(rng) & kk(instruction);
}

inline auto display_sprite(State& state, Instruction_t instruction) -> void
{
  auto const at       = std::pair{x(instruction), y(instruction)};
  auto const length   = n(instruction);
  auto const location = state.index_register;

  for (auto i = Address_t{0x0}; i < length; ++i) {
    auto const bits = state.memory[location + i];
    for (auto j = 0x0; j < 8; ++j) {
      state.screen_buffer[at.second][at.first + j] = 0x1 & (bits >> j);
    }
  }

  // each byte is 8 pixels to display on screen, display if true
  //  this is possibly displaying multiple bytes, I think each byte is one step
  //  down on the y direction.

  // so you need a way to iterate through the bits of each byte in msb->lsb
  // order

  // at each bit, you modify a particular location in the screen buffer
  // xor it with current value, if previous value way 1, then set VF to 1 if any
  // bits were flipped, and set it to zero otherwise.

  // std::cout << (int)at.first << ' ' << (int)at.second << std::endl;
  // std::cout << (int)length << std::endl;
  // std::cout << (int)location << std::endl;
  // xor onto screen, if collision, set VF?, wrap if x goes past or y if y goes
  // past.
}

inline auto skip_if_pressed(State& state, Instruction_t instruction) -> void
{
  auto const key = state.general_purpose_registers[x(instruction)];
  if (state.pressed_key.has_value() && state.pressed_key.value() == key) {
    state.program_counter += 2;
  }
}

inline auto skip_if_not_pressed(State& state, Instruction_t instruction) -> void
{
  auto const key = state.general_purpose_registers[x(instruction)];
  if (state.pressed_key.has_value() && state.pressed_key.value() != key) {
    state.program_counter += 2;
  }
}

inline auto set_from_delay_timer(State& state, Instruction_t instruction)
  -> void
{
  state.general_purpose_registers[x(instruction)] =
    state.delay_timer_register.value;
}

inline auto wait_for_keypress(State& state, Instruction_t instruction) -> void
{
  // TODO remove this incrememt
  state.program_counter += 2;
  // TODO Use Escape method to check and immediatly return if a key is
  // pressed, this could be wrapped in a method that also filters out
  // non-chip8 keys and returns nullopt otherwise.
  // Then you can store the value of the key in Vx, and increment the program
  // couter, since this won't do it otherwise.
}

inline auto set_delay_timer(State& state, Instruction_t instruction) -> void
{
  state.delay_timer_register.value =
    state.general_purpose_registers[x(instruction)];
}

inline auto set_sound_timer(State& state, Instruction_t instruction) -> void
{
  state.sound_timer_register.value =
    state.general_purpose_registers[x(instruction)];
}

inline auto add_to_index_register(State& state, Instruction_t instruction)
  -> void
{
  state.index_register += state.general_purpose_registers[x(instruction)];
}

inline auto set_index_register_to_digit_sprite(State& state,
                                               Instruction_t instruction)
  -> void
{
  // TODO figure out the hard coded locations and assign, maybe a map? though
  // maybe that's too much.
  // state.index_register += state.general_purpose_registers[x(instruction)];
}

inline auto store_bcd_representation(State& state, Instruction_t instruction)
  -> void
{
  // Find the hundreds, tens, ones of Vx
  // Store these in I, I+1 and I+2 in memory
  auto const vx       = state.general_purpose_registers[x(instruction)];
  auto const hundreds = vx % 100;
  auto const tens     = (vx / 10) % 10;
  auto const ones     = (vx / 100);
  // std::cout << hundreds << ' ' << tens << ' ' << ones << std::endl;
  state.memory[state.index_register]     = hundreds;
  state.memory[state.index_register + 1] = tens;
  state.memory[state.index_register + 2] = ones;
}

inline auto registers_to_memory(State& state, Instruction_t instruction) -> void
{
  for (auto i = 0; i <= x(instruction); ++i) {
    state.memory[state.index_register + i] = state.general_purpose_registers[i];
  }
}

inline auto memory_to_registers(State& state, Instruction_t instruction) -> void
{
  for (auto i = 0; i <= x(instruction); ++i) {
    state.general_purpose_registers[i] = state.memory[state.index_register + i];
  }
}

}  // namespace

namespace chip8 {

/// Return the next program counter address.
inline auto process_instruction(State& state, Instruction_t instruction)
  -> Address_t
{
  // print_registers(state);
  switch (opcode(instruction)) {
    case 0x0:
      if (instruction == 0x00E0) {
        clear_display(state);
      }
      else if (instruction == 0x00EE) {
        subroutine_return(state);
      }
      else {
        // System machine code jump, not used in emulated environment.
      }
      break;
    case 0x1: jump_to_address(state, instruction); break;
    case 0x2: call_subroutine(state, instruction); break;
    case 0x3: skip_if_equal_rb(state, instruction); break;
    case 0x4: skip_if_not_equal_rb(state, instruction); break;
    case 0x5: skip_if_equal_rr(state, instruction); break;
    case 0x6: set_register(state, instruction); break;
    case 0x7: add_register(state, instruction); break;
    case 0x8:
      switch (n(instruction)) {
        case 0x0: load_y_to_x(state, instruction); break;
        case 0x1: bitwise_or(state, instruction); break;
        case 0x2: bitwise_and(state, instruction); break;
        case 0x3: bitwise_xor(state, instruction); break;
        case 0x4: add_with_carry(state, instruction); break;
        case 0x5: subtract_with_not_borrow(state, instruction); break;
        case 0x6: shift_right(state, instruction); break;
        case 0x7: rsubtract_with_not_borrow(state, instruction); break;
        case 0xE: shift_left(state, instruction); break;
      }
      break;
    case 0x9: skip_if_not_equal_rr(state, instruction); break;
    case 0xA: set_index_register(state, instruction); break;
    case 0xB: jump_to_nnn_plus_v0(state, instruction); break;
    case 0xC: random_byte(state, instruction); break;
    case 0xD: display_sprite(state, instruction); break;
    case 0xE:
      switch (kk(instruction)) {
        case 0x9E: skip_if_pressed(state, instruction); break;
        case 0xA1: skip_if_not_pressed(state, instruction); break;
        default: throw unknown_instruction_exception(instruction);
      }
      break;
    case 0xF:
      switch (kk(instruction)) {
        case 0x07: set_from_delay_timer(state, instruction); break;
        case 0x0A:
          wait_for_keypress(state, instruction);
          // FIXME not great, but how else do you get timers async?
          return state.program_counter;
        case 0x15: set_delay_timer(state, instruction); break;
        case 0x18: set_sound_timer(state, instruction); break;
        case 0x1E: add_to_index_register(state, instruction); break;
        case 0x29:
          set_index_register_to_digit_sprite(state, instruction);
          break;
        case 0x33: store_bcd_representation(state, instruction); break;
        case 0x55: registers_to_memory(state, instruction); break;
        case 0x65: memory_to_registers(state, instruction); break;
        default: throw unknown_instruction_exception(instruction);
      }
  }
  return state.program_counter + 2;
}

/// Return the 2 byte instruction at the current program counter.
/// Return std::nullopt if the program counter points to an invalid address.
inline auto get_instruction(State const& state) -> std::optional<Instruction_t>
{
  if (state.program_counter + 1 >= state.memory.size()) {
    return std::nullopt;
  }
  return (std::uint16_t(state.memory[state.program_counter]) << 8) |
         state.memory[state.program_counter + 1];
}

}  // namespace chip8

#endif  // INSTRUCTIONS_HPP
