#pragma once

namespace Beer::Rendering
{
#define PAD_4(id) float _pad_##id;
#define PAD_8(id) float _pad_##id[2];
#define PAD_12(id) float _pad_##id[3];

#define ALIGN_16 alignas(16)

} // namespace Beer::Rendering
