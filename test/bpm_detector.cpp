#include <mc/core/config.hpp>

#include <mc/dsp/convolution.hpp>
#include <mc/dsp/wavelets.hpp>

#include <mc/core/algorithm.hpp>
#include <mc/core/AudioFile.h>
#include <mc/core/cmath.hpp>
#include <mc/core/format.hpp>
#include <mc/core/numeric.hpp>
#include <mc/core/utility.hpp>
#include <mc/testing/test.hpp>

#include <fmt/printf.h>

using namespace mc;

template<typename It>
auto mean(It f, It l) -> float
{
    auto const sum = std::accumulate(f, l, 0.0F);
    return sum / static_cast<float>(std::distance(f, l));
}

auto peakDetect(mc::Span<float> data) -> std::size_t
{
    auto peaks = std::minmax_element(data.begin(), data.end());
    if (std::abs(*peaks.first) >= std::abs(*peaks.second)) {
        return std::distance(data.begin(), peaks.first);
    }
    return std::distance(data.begin(), peaks.second);
}

struct BpmDetect
{
    BpmDetect(std::size_t n, std::size_t levels)
        : wave_{"db4"}
        , wt_{wave_, "dwt", n, levels}
    {
        wt_.extension(dsp::SignalExtension::symmetric);
        wt_.convMethod(dsp::ConvolutionMethod::direct);
    }

    [[nodiscard]] auto perform(mc::Span<float> input, float sampleRate) -> float
    {

        auto const levels        = 4;
        auto const maxDecimation = std::pow(2.0F, levels - 1);
        auto const minNdx = std::floor(60.0F / 220.0F * (sampleRate / maxDecimation));
        auto const maxNdx = std::floor(60.0F / 40.0F * (sampleRate / maxDecimation));

        auto cA = mc::Span<float>{};
        auto cD = mc::Span<float>{};

        auto cDMinlen = 0.0F;
        auto cDSum    = Vector<float>{};
        dwt(wt_, input.data());
        for (auto loop{0}; loop < levels; ++loop) {
            if (loop == 0) {
                // dwt(wt_, input.data());
                cA       = wt_.approx();
                cD       = wt_.detail(loop + 1);
                cDMinlen = static_cast<float>(mc::size(cD)) / maxDecimation + 1.0F;
                cDSum.resize(static_cast<std::size_t>(std::floor(cDMinlen)));
                ranges::fill(cDSum, 0.0F);
            } else {
                // dwt(wt_, cA.data());
                cA = wt_.approx();
                cD = wt_.detail(loop + 1);
            }

            // 2) Filter
            // cD = signal.lfilter([0.01], [1 - 0.99], cD)

            // 4) Subtract out the mean.
            // cD = cD - np.mean(cD)
            auto const m = mean(std::begin(cD), std::end(cD));
            ranges::transform(cD, std::begin(cD), [m](auto v) { return v - m; });

            // 5) Decimate for reconstruction later.
            // cD = abs(cD[:: (2 ** (levels - loop - 1))])
            cD = cD.subspan(0, static_cast<std::size_t>(std::pow(2, levels - loop - 1)));
            ranges::transform(cD, std::begin(cD), [](auto v) { return std::abs(v); });

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
        ranges::transform(cA, begin(cA), [](auto v) { return std::abs(v); });
        auto const m = mean(std::begin(cA), std::end(cA));
        ranges::transform(cA, begin(cA), [m](auto v) { return v - m; });
        cDSum.insert(
            begin(cDSum),
            std::begin(cA),
            std::next(std::begin(cA), static_cast<size_t>(std::floor(cDMinlen)))
        );

        // # ACF
        // correl = np.correlate(cD_sum, cD_sum, "full")
        // cD_sum.clear();
        std::copy(
            std::begin(wt_.output()),
            std::begin(wt_.output()) + wt_.outlength,
            std::back_inserter(cDSum)
        );
        // std::transform(begin(cD_sumf_), end(cD_sumf_), begin(cD_sumf_), [](auto v) {
        // return std::abs(v); }); auto const m = mean(begin(cD_sumf_), end(cD_sumf_));
        // std::transform(begin(cD_sumf_), end(cD_sumf_), begin(cD_sumf_), [m](auto v) {
        // return v - m; });

        auto s = dsp::FloatSignal(cDSum.data(), cDSum.size());
        auto x = dsp::OverlapSaveConvolver(s, s);
        x.crossCorrelate();
        auto correl = x.extractResult();

        auto midpoint = static_cast<std::size_t>(std::floor(correl.size() / 2.0F));
        auto correlMidpointTmp
            = mc::Span<float>{correl.data(), correl.size()}.subspan(midpoint);
        auto const peakNdx = peakDetect(correlMidpointTmp.subspan(minNdx, maxNdx - minNdx));

        auto const peakNdxAdjusted = peakNdx + minNdx;
        auto const bpm             = 60.0F / peakNdxAdjusted * (sampleRate / maxDecimation);
        return bpm;
    }

private:
    dsp::Wavelet wave_;
    dsp::WaveletTransform wt_;
};

static auto median(mc::Span<float> data) -> float
{
    if (mc::empty(data)) { return 0.0; }

    ranges::sort(data);
    auto const size = mc::size(data);
    auto const mid  = size / 2;
    return size % 2 == 0 ? (data[mid] + data[mid - 1]) / 2 : data[mid];
}

static auto mode(mc::Span<float const> arr) -> float
{
    auto const n   = arr.size();
    float count    = 1;
    float countmax = 0;
    float current  = arr[0];
    float moda     = 0;
    for (std::size_t i = 1; i < n; i++) {
        if (arr[i] == current) {
            count++;
        } else if (count > countmax) {
            countmax = count;
            count    = 1;
            moda     = arr[i - 1];
        }
        current = arr[i];
    }
    return moda;
}

auto main(int argc, char** argv) -> int
{
    if (argc != 2) {
        fmt::print("no file path provided");
        return EXIT_FAILURE;
    }

    AudioFile<float> audioFile;
    audioFile.load(argv[1]);
    audioFile.printSummary();

    auto const fs = static_cast<float>(audioFile.getSampleRate());
    // auto const numSamples = static_cast<size_t>(audioFile.getNumSamplesPerChannel());

    auto channel
        = mc::Span<float>(audioFile.samples[0].data(), audioFile.samples[0].size());
    // channel = channel.subspan(static_cast<std::size_t>(mc::size(audioFile.samples[0]) *
    // (0.05 / 100.0))); channel =
    // channel.last(static_cast<std::size_t>(mc::size(audioFile.samples[0]) * (0.05 /
    // 100.0)));

    auto const windowSize     = static_cast<std::size_t>(std::floor(3.0 * fs));
    auto const maxWindowIndex = mc::size(channel) / windowSize;

    auto sampsNdx = 0U;
    auto bpms     = Vector<float>{};
    for (auto windowNdx{0U}; windowNdx < maxWindowIndex; ++windowNdx) {
        if (sampsNdx + windowSize >= mc::size(channel)) {
            fmt::print("ERROR");
            continue;
        }

        auto detector  = BpmDetect{windowSize, 4};
        auto subBuffer = channel.subspan(sampsNdx, windowSize);
        auto bpm       = detector.perform(subBuffer, fs);

        fmt::printf("BPM:%.1f\n", std::round(bpm));
        if (bpm == 0.0) { continue; }

        bpms.push_back(bpm);
        sampsNdx += windowSize;
    }

    auto outOfRange = [](auto b) { return b < 100.0 || b > 200.0; };
    bpms.erase(ranges::remove_if(bpms, outOfRange), end(bpms));

    fmt::printf("Detected BPM (median): %.1f\n", std::round(median(bpms)));
    fmt::printf("Detected BPM (mode): %.1f\n", std::round(mode(bpms)));
    return 0;
}
