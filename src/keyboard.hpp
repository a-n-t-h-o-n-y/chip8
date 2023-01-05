#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP
#include <cstdint>
#include <optional>

namespace chip8 {

inline auto get_keyboard_state() -> std::optional<std::uint8_t>
{
  // TODO
  // Use the escape library to get current key(s) being pressed
  // can you just get the most recent key pressed?
  // using Key_mode::Alternate in escape read(timeout) fn you'll get an event
  // variant that you can visit to set the state's keyboard to nullopt on
  // release. set to key on press, if its a valid key, same with release, it
  // should be a valid key being released, otherwise nothing
  return std::nullopt;
}

}  // namespace chip8
#endif  // KEYBOARD_HPP
