#pragma once

#include "lt/dsp/convolution/FFTConvolver.hpp"
#include "lt/dsp/wavelets/Wavelet.hpp"

#include "lt/preprocessor.hpp"
#include "lt/span.hpp"
#include "lt/string.hpp"

namespace lt::dsp
{

struct WaveletPacketTransform
{
    WaveletPacketTransform(Wavelet* wave, std::size_t siglength, std::size_t j);

    LT_NODISCARD auto wave() const noexcept -> Wavelet const& { return *wave_; }
    LT_NODISCARD auto signalLength() const noexcept -> int { return signalLength_; }

    FFTConvolver* cobj{};
    int outlength{};  // Length of the output DWT vector
    int lenlength{};  // Length of the Output Dimension Vector "length"
    int J{};          // Number of decomposition Levels
    int MaxIter{};    // Maximum Iterations J <= MaxIter
    std::string ext;  // Type of Extension used - "per" or "sym"
    std::string entropy;
    float eparam{};

    int N{};  //
    int nodes{};
    int length[102]{};
    float* output{};
    float* costvalues{};
    float* basisvector{};
    int* nodeindex{};
    int* numnodeslevel{};
    int* coeflength{};
    std::unique_ptr<float[]> params;

private:
    Wavelet* wave_{nullptr};
    int signalLength_{};  // Length of the original signal.
};

auto dwt(WaveletPacketTransform& wt, float const* inp) -> void;
auto idwt(WaveletPacketTransform& wt, float* dwtop) -> void;

auto setDWPTExtension(WaveletPacketTransform& wt, char const* extension) -> void;
auto setDWPTEntropy(WaveletPacketTransform& wt, char const* entropy, float eparam) -> void;
auto getDWPTNodelength(WaveletPacketTransform& wt, int x) -> int;

auto summary(WaveletPacketTransform const& wt) -> void;

}  // namespace lt::dsp