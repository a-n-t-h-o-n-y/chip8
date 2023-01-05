#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <cstdint>

namespace chip8 {

inline constexpr auto INSTRUCTION_OFFSET  = 0x200u;
inline constexpr auto MEMORY_AMOUNT       = 0x1000u;
inline constexpr auto DIGIT_SPRITE_OFFSET = std::uint16_t{0x0};

static_assert(MEMORY_AMOUNT > INSTRUCTION_OFFSET);

}  // namespace chip8
#endif  // CONSTANTS_HPP
