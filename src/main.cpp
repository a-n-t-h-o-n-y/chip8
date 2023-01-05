#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "clock.hpp"
#include "constants.hpp"
#include "initialize.hpp"
#include "instructions.hpp"
#include "keyboard.hpp"
#include "screen.hpp"
#include "timer.hpp"

struct Options {
  std::string rom_filepath;
  std::optional<std::uint16_t> clock_hz;
};

/// Usage: chip8 <rom> [--clock uint16_t]
auto parse_command_line(int argc, char* argv[]) -> Options
{
  if (argc < 2) {
    throw std::runtime_error{"Usage: chip8 <rom> [--clock uint16_t]"};
  }
  auto const args = std::vector(argv, std::next(argv, argc));
  auto result     = Options{args[1], std::nullopt};
  auto const at   = std::ranges::find(args, std::string{"--clock"});
  if (at != std::end(args) && std::next(at) != std::end(args)) {
    try {
      auto const clock = std::stoi(*std::next(at));
      if (clock >= std::pow(2, 16) || clock < 0) {
        throw std::runtime_error{"--clock argument must fit in a uint16_t."};
      }
      result.clock_hz = static_cast<std::uint16_t>(clock);
    }
    catch (std::invalid_argument const&) {
      throw std::runtime_error{"--clock argument must be an integer."};
    }
    catch (std::out_of_range const&) {
      throw std::runtime_error{"--clock argument must fit in a uint16_t."};
    }
  }
  else if (at != std::end(args) && std::next(at) == std::end(args)) {
    throw std::runtime_error{"--clock is missing its integer argument."};
  }
  return result;
}

auto main(int argc, char* argv[]) -> int
{
  using namespace chip8;
  try {
    auto const options  = parse_command_line(argc, argv);
    auto const clock_fn = make_clock_fn(options.clock_hz);
    auto state          = initialize_state(load_program(argv[1]));
    while (true) {
      auto const start       = Clock_t::now();
      auto const instruction = get_instruction(state);
      if (!instruction.has_value()) {
        break;
      }
      state.program_counter          = process_instruction(state, *instruction);
      auto const instruction_runtime = clock_fn(*instruction);

      update_timer(state.delay_timer_register);
      update_timer(state.sound_timer_register);

      state.pressed_key = get_keyboard_state();

      if (is_graphics_instruction(*instruction)) {
        update_graphics(state);
      }

      // Wait out for rest of instruction cycle time.
      auto const elapsed = Clock_t::now() - start;
      std::this_thread::sleep_for(instruction_runtime - elapsed);
    }

    std::cout << "Done.\n";
    return 0;
  }
  catch (std::exception const& e) {
    std::cerr << "Fatal Error: " << e.what() << '\n';
    return 1;
  }
}
