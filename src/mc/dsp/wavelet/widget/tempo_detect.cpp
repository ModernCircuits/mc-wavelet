#include "tempo_detect.hpp"

#include <mc/dsp/convolution.hpp>

#include <mc/core/algorithm.hpp>
#include <mc/core/cmath.hpp>
#include <mc/core/format.hpp>
#include <mc/core/numeric.hpp>
#include <mc/core/utility.hpp>

namespace mc::dsp {

template<typename It>
auto mean(It f, It l) -> float
{
    auto const sum = std::accumulate(f, l, 0.0F);
    return sum / static_cast<float>(std::distance(f, l));
}

auto peakDetect(Span<float> data) -> std::size_t
{
    auto peaks = std::minmax_element(data.begin(), data.end());
    if (std::abs(*peaks.first) >= std::abs(*peaks.second)) {
        return std::distance(data.begin(), peaks.first);
    }
    return std::distance(data.begin(), peaks.second);
}

TempoDetect::TempoDetect(std::size_t n, std::size_t levels)
    : _wave{"db4"}
    , _wt{_wave, "dwt", n, levels}
{
    _wt.extension(dsp::SignalExtension::symmetric);
    _wt.convMethod(dsp::ConvolutionMethod::direct);
}

auto TempoDetect::operator()(Span<float> input, float sampleRate) -> float
{

    auto const levels        = _wt.levels();
    auto const maxDecimation = std::pow(2.0F, static_cast<float>(levels - 1));
    auto const minNdx        = std::floor(60.0F / 220.0F * (sampleRate / maxDecimation));
    auto const maxNdx        = std::floor(60.0F / 40.0F * (sampleRate / maxDecimation));

    auto cA = Span<float>{};
    auto cD = Span<float>{};

    auto cDMinlen = 0.0F;
    auto cDSum    = Vector<float>{};
    dwt(_wt, input.data());
    for (auto loop{0}; loop < levels; ++loop) {
        if (loop == 0) {
            // dwt(wt_, input.data());
            cA       = _wt.approx();
            cD       = _wt.detail(loop + 1);
            cDMinlen = static_cast<float>(mc::size(cD)) / maxDecimation + 1.0F;
            cDSum.resize(static_cast<std::size_t>(std::floor(cDMinlen)));
            ranges::fill(cDSum, 0.0F);
        } else {
            // dwt(wt_, cA.data());
            cA = _wt.approx();
            cD = _wt.detail(loop + 1);
        }

        // 2) Filter
        // cD = signal.lfilter([0.01], [1 - 0.99], cD)

        // 4) Subtract out the mean.
        // cD = cD - np.mean(cD)
        auto const m = mean(std::begin(cD), std::end(cD));
        ranges::transform(cD, begin(cD), [m](auto v) { return v - m; });

        // 5) Decimate for reconstruction later.
        // cD = abs(cD[:: (2 ** (levels - loop - 1))])
        cD = cD.subspan(0, static_cast<std::size_t>(std::pow(2, levels - loop - 1)));
        ranges::transform(cD, begin(cD), [](auto v) { return std::abs(v); });

        // 6) Recombine the signal before ACF
        //    Essentially, each level the detail coefs (i.e. the HPF values)
        //    are concatenated to the beginning of the array
        // cD_sum = cD[0: math.floor(cD_minlen)] + cD_sum
        cDSum.insert(
            begin(cDSum),
            std::begin(cD),
            std::next(std::begin(cD), static_cast<size_t>(std::floor(cDMinlen)))
        );
    }

    // if [b for b in cA if b != 0.0] == []:
    //     return no_audio_data()
    if (ranges::none_of(cA, [](auto s) { return s != 0.0F; })) { return 0.0F; }

    // # Adding in the approximate data as well...
    // cA = signal.lfilter([0.01], [1 - 0.99], cA)
    // cA = abs(cA)
    // cA = cA - np.mean(cA)
    // cD_sum = cA[0: math.floor(cD_minlen)] + cD_sum
    ranges::transform(cA, std::begin(cA), [](auto v) { return std::abs(v); });
    auto const m = mean(std::begin(cA), std::end(cA));
    ranges::transform(cA, std::begin(cA), [m](auto v) { return v - m; });
    cDSum.insert(
        begin(cDSum),
        std::begin(cA),
        std::next(std::begin(cA), static_cast<size_t>(std::floor(cDMinlen)))
    );

    // # ACF
    // correl = np.correlate(cD_sum, cD_sum, "full")
    // cD_sum.clear();
    std::copy(
        std::begin(_wt.output()),
        std::begin(_wt.output()) + _wt.outlength,
        std::back_inserter(cDSum)
    );
    // std::transform(begin(cD_sumf_), end(cD_sumf_), begin(cD_sumf_), [](auto v) { return
    // std::abs(v); }); auto const m = mean(begin(cD_sumf_), end(cD_sumf_));
    // std::transform(begin(cD_sumf_), end(cD_sumf_), begin(cD_sumf_), [m](auto v) { return
    // v - m; });

    auto s = dsp::FloatSignal(cDSum.data(), cDSum.size());
    auto x = dsp::OverlapSaveConvolver(s, s);
    x.crossCorrelate();
    auto correl = x.extractResult();

    auto midpoint          = static_cast<std::size_t>(std::floor(correl.size() / 2.0F));
    auto correlMidpointTmp = Span<float>{correl.data(), correl.size()}.subspan(midpoint);
    auto const peakNdx
        = peakDetect(correlMidpointTmp.subspan((size_t)minNdx, (size_t)(maxNdx - minNdx)));

    auto const peakNdxAdjusted = peakNdx + minNdx;
    auto const bpm             = 60.0F / peakNdxAdjusted * (sampleRate / maxDecimation);
    return bpm;
}

}  // namespace mc::dsp