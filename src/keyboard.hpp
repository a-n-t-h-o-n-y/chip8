#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP
#include <chrono>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <variant>

#include <esc/event.hpp>
#include <esc/io.hpp>

#include "types.hpp"

namespace chip8 {

/// Implements an auto-release keyboard that reads from STDIN
/** Reading key release events from terminal is a pain and requires superuser,
 *  this is a 'good enough' solution for chip8 interpreter.
 *  Important that auto-repeat keys overlap to provide illusion of continuous
 *  key press. Does not support simultaneous keys, a 'monophonic' keyboard.
 */
template <int AutoReleaseMS>
class Keyboard {
 public:
  /// Returns std::nullopt if no key is pressed, otherwise the chip8 keyvalue
  /// in the range [0x0 - 0xF].
  auto get_state() -> std::optional<std::uint8_t>
  {
    auto const state = this->get_keyboard_state([] { return esc::read(0); });
    if (state.has_value()) {
      this->press_key(*state);
    }
    else if (key_.has_value() && (Clock_t::now() - last_press_) >=
                                   std::chrono::milliseconds{AutoReleaseMS}) {
      key_ = std::nullopt;
    }
    return key_;
  }

  /// Returns the chip8 keyvalue in the range [0x0 - 0xF], waits for keypress.
  auto get_state_blocking() -> std::uint8_t
  {
    auto state = this->get_state();  // non-blocking call first.
    if (state.has_value()) {
      return *state;
    }
    do {
      state =
        this->get_keyboard_state([] { return std::optional{esc::read()}; });
    } while (!state.has_value());
    this->press_key(*state);
    return *key_;
  }

 private:
  /// Records the key press and resets the last_press_ time.
  auto press_key(std::uint8_t key) -> void
  {
    key_        = key;
    last_press_ = Clock_t::now();
  }

  template <typename ReadFn>
  auto get_keyboard_state(ReadFn&& read_fn) -> std::optional<std::uint8_t>
  {
    if (auto const event = read_fn(); event.has_value()) {
      return std::visit(
        [](auto&& x) -> std::optional<std::uint8_t> {
          using T = std::decay_t<decltype(x)>;
          if constexpr (std::is_same_v<T, esc::Key_press>) {
            switch (x.key) {
              using esc::Key;
              case Key::One: return 0x1u;
              case Key::Two: return 0x2u;
              case Key::Three: return 0x3u;
              case Key::Four: return 0xCu;
              case Key::q: return 0x4u;
              case Key::w: return 0x5u;
              case Key::e: return 0x6u;
              case Key::r: return 0xDu;
              case Key::a: return 0x7u;
              case Key::s: return 0x8u;
              case Key::d: return 0x9u;
              case Key::f: return 0xEu;
              case Key::z: return 0xAu;
              case Key::x: return 0x0u;
              case Key::c: return 0xBu;
              case Key::v: return 0xFu;
            }
          }
          return std::nullopt;
        },
        *event);
    }
    return std::nullopt;
  }

 private:
  std::optional<std::uint8_t> key_;
  std::chrono::time_point<Clock_t> last_press_;
};

}  // namespace chip8
#endif  // KEYBOARD_HPP
