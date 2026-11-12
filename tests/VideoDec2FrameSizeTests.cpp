#include "libs/libVideoDec2.cpp"

#include <cstdio>
#include <cstdlib>

namespace Loader {
void SymbolDatabase::Add(const SymbolResolve &, uint64_t, const std::string &) {
}
} // namespace Loader

using namespace Libs::VideoDec2;

namespace {

void Check(bool value, const char *text) {
  if (!value) {
    std::fprintf(stderr, "VideoDec2FrameSizeTests: failed: %s\n", text);
    std::exit(1);
  }
}

size_t FrameSize(uint32_t codec_type, uint32_t profile, int32_t width,
                 int32_t height) {
  Videodec2DecoderConfigInfo config{};
  config.this_size = sizeof(config);
  config.resource_type = VIDEODEC2_RESOURCE_TYPE_COMPUTE;
  config.codec_type = codec_type;
  config.profile = profile;
  config.max_frame_width = width;
  config.max_frame_height = height;
  config.max_dpb_frame_count = -1;
  config.decode_input_queue_depth = 4;

  Videodec2DecoderMemoryInfo memory{sizeof(memory)};
  Check(QueryDecoderMemoryInfo(&config, &memory) == 0,
        "query 1920x1088 frame size");
  return memory.max_frame_buffer_size;
}

} // namespace

int main() {
  Check(FrameSize(VIDEODEC2_CODEC_TYPE_AVC, 100, 1920, 1088) == 3346432u,
        "AVC 8-bit output size");
  Check(FrameSize(VIDEODEC2_CODEC_TYPE_HEVC, 1, 1920, 1088) == 3346432u,
        "HEVC 8-bit output size");
  Check(FrameSize(VIDEODEC2_CODEC_TYPE_HEVC, 2, 1920, 1088) == 6688768u,
        "HEVC 10-bit output size");
  Check(FrameSize(VIDEODEC2_CODEC_TYPE_HEVC, 2, 3840, 2160) == 24887296u,
        "HEVC 10-bit 4K output size");
  Check(FrameSize(VIDEODEC2_CODEC_TYPE_VP9, 0, 1920, 1088) == 3350528u,
        "VP9 8-bit output size");
  Check(FrameSize(VIDEODEC2_CODEC_TYPE_VP9, 2, 1920, 1088) == 6692864u,
        "VP9 10-bit output size");
  Check(FrameSize(VIDEODEC2_CODEC_TYPE_VP9, 0, 1920, 1090) == 3547136u,
        "VP9 64-line height alignment");
  Check(FrameSize(VIDEODEC2_CODEC_TYPE_HEVC, 1, -1, -1) ==
            VIDEODEC2_MIN_MEMORY_SIZE,
        "unbounded dimensions retain decoder memory minimum");

  std::puts("VideoDec2FrameSizeTests: all passed");
  return 0;
}
