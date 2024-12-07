#pragma once
#include <imgui.h>

// References
// https://en.wikipedia.org/wiki/ANSI_escape_code

namespace Mi {
namespace Color {
inline constexpr ImU32 Transparent = IM_COL32(0, 0, 0, 0);

namespace Vga {
inline constexpr ImU32 Black = IM_COL32(0, 0, 0, 255);
inline constexpr ImU32 Red = IM_COL32(170, 0, 0, 255);
inline constexpr ImU32 Green = IM_COL32(0, 170, 0, 255);
inline constexpr ImU32 Yellow = IM_COL32(170, 85, 0, 255);
inline constexpr ImU32 Blue = IM_COL32(0, 0, 170, 255);
inline constexpr ImU32 Magenta = IM_COL32(170, 0, 170, 255);
inline constexpr ImU32 Cyan = IM_COL32(0, 170, 170, 255);
inline constexpr ImU32 White = IM_COL32(170, 170, 170, 255);

inline constexpr ImU32 BrightBlack = IM_COL32(85, 85, 85, 255);
inline constexpr ImU32 BrightRed = IM_COL32(255, 85, 85, 255);
inline constexpr ImU32 BrightGreen = IM_COL32(85, 255, 85, 255);
inline constexpr ImU32 BrightYellow = IM_COL32(255, 255, 85, 255);
inline constexpr ImU32 BrightBlue = IM_COL32(85, 85, 255, 255);
inline constexpr ImU32 BrightMagenta = IM_COL32(255, 85, 255, 255);
inline constexpr ImU32 BrightCyan = IM_COL32(85, 255, 255, 255);
inline constexpr ImU32 BrightWhite = IM_COL32(255, 255, 255, 255);
} // namespace Vga

} // namespace Color
} // namespace Mi
