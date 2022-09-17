#include "Wavelet.hpp"

#include <mc/dsp/convolution/FFTConvolver.hpp>
#include <mc/dsp/wavelets/filters/coif.hpp>
#include <mc/dsp/wavelets/filters/daubechies.hpp>
#include <mc/dsp/wavelets/filters/meyer.hpp>
#include <mc/dsp/wavelets/filters/sym.hpp>

#include <mc/core/cmath.hpp>
#include <mc/core/cstring.hpp>
#include <mc/core/format.hpp>
#include <mc/core/iterator.hpp>
#include <mc/core/memory.hpp>
#include <mc/core/numbers.hpp>
#include <mc/core/raise.hpp>
#include <mc/core/stdexcept.hpp>
#include <mc/core/string_view.hpp>
#include <mc/core/utility.hpp>

namespace mc::dsp {

namespace {

template<typename T>
auto qmfEven(T const* in, int n, T* out)
{
    for (auto count = 0; count < n; ++count) {
        auto evenIndex = count % 2 == 0;
        out[count]     = in[n - count - 1] * (evenIndex ? T{1} : T{-1});
    }
}

template<typename T>
auto qmfWrev(T const* in, int n, T* out)
{
    auto tmp = makeUnique<T[]>(n);
    qmfEven(in, n, tmp.get());
    std::reverse_copy(tmp.get(), tmp.get() + n, out);
}

auto waveletFilterLength(char const* name) -> int
{
    auto const len = static_cast<int>(std::strlen(name));
    auto n         = 0;

    if (name == StringView{"haar"} || name == StringView{"db1"}) { return 2; }
    if (len > 2 && strstr(name, "db") != nullptr) {
        auto newStr = makeUnique<char[]>((len - 2 + 1));
        for (auto i = 2; i < len + 1; i++) { newStr[i - 2] = name[i]; }

        n = atoi(newStr.get());
        if (n > 38) { raise<InvalidArgument>("wavelet filter not in database"); }

        return n * 2;
    }

    if (len > 4 && strstr(name, "coif") != nullptr) {
        auto newStr = makeUnique<char[]>((len - 4 + 1));
        for (auto i = 4; i < len + 1; i++) { newStr[i - 4] = name[i]; }

        n = atoi(newStr.get());
        if (n > 17) { raise<InvalidArgument>("wavelet filter not in database"); }

        return n * 6;
    }
    if (len > 3 && strstr(name, "sym") != nullptr) {
        auto newStr = makeUnique<char[]>((len - 3 + 1));
        for (auto i = 3; i < len + 1; i++) { newStr[i - 3] = name[i]; }

        n = atoi(newStr.get());
        if (n > 20 || n < 2) { raise<InvalidArgument>("wavelet filter not in database"); }

        return n * 2;
    }
    if (strcmp(name, "meyer") == 0) { return 102; }

    raise<InvalidArgument>("wavelet filter not in database");
}

auto fillDauberchies(char const* name, float* lp1, float* hp1, float* lp2, float* hp2)
{
    using namespace std::string_view_literals;

    auto const* coeffs = static_cast<float*>(nullptr);
    if (name == "haar"sv || name == "db1"sv) { coeffs = std::data(daubechies1<float>); }
    if (name == "db2"sv) { coeffs = std::data(daubechies2<float>); }
    if (name == "db3"sv) { coeffs = std::data(daubechies3<float>); }
    if (name == "db4"sv) { coeffs = std::data(daubechies4<float>); }
    if (name == "db5"sv) { coeffs = std::data(daubechies5<float>); }
    if (name == "db6"sv) { coeffs = std::data(daubechies6<float>); }
    if (name == "db7"sv) { coeffs = std::data(daubechies7<float>); }
    if (name == "db8"sv) { coeffs = std::data(daubechies8<float>); }
    if (name == "db9"sv) { coeffs = std::data(daubechies9<float>); }
    if (name == "db10"sv) { coeffs = std::data(daubechies10<float>); }
    if (name == "db11"sv) { coeffs = std::data(daubechies11<float>); }
    if (name == "db12"sv) { coeffs = std::data(daubechies12<float>); }
    if (name == "db13"sv) { coeffs = std::data(daubechies13<float>); }
    if (name == "db14"sv) { coeffs = std::data(daubechies14<float>); }
    if (name == "db15"sv) { coeffs = std::data(daubechies15<float>); }
    if (name == "db16"sv) { coeffs = std::data(daubechies16<float>); }
    if (name == "db17"sv) { coeffs = std::data(daubechies17<float>); }
    if (name == "db18"sv) { coeffs = std::data(daubechies18<float>); }
    if (name == "db19"sv) { coeffs = std::data(daubechies19<float>); }
    if (name == "db20"sv) { coeffs = std::data(daubechies20<float>); }
    if (name == "db21"sv) { coeffs = std::data(daubechies21<float>); }
    if (name == "db22"sv) { coeffs = std::data(daubechies22<float>); }
    if (name == "db23"sv) { coeffs = std::data(daubechies23<float>); }
    if (name == "db24"sv) { coeffs = std::data(daubechies24<float>); }
    if (name == "db25"sv) { coeffs = std::data(daubechies25<float>); }
    if (name == "db26"sv) { coeffs = std::data(daubechies26<float>); }
    if (name == "db27"sv) { coeffs = std::data(daubechies27<float>); }
    if (name == "db28"sv) { coeffs = std::data(daubechies28<float>); }
    if (name == "db29"sv) { coeffs = std::data(daubechies29<float>); }
    if (name == "db30"sv) { coeffs = std::data(daubechies30<float>); }
    if (name == "db31"sv) { coeffs = std::data(daubechies31<float>); }
    if (name == "db32"sv) { coeffs = std::data(daubechies32<float>); }
    if (name == "db33"sv) { coeffs = std::data(daubechies33<float>); }
    if (name == "db34"sv) { coeffs = std::data(daubechies34<float>); }
    if (name == "db35"sv) { coeffs = std::data(daubechies35<float>); }
    if (name == "db36"sv) { coeffs = std::data(daubechies36<float>); }
    if (name == "db37"sv) { coeffs = std::data(daubechies37<float>); }
    if (name == "db38"sv) { coeffs = std::data(daubechies38<float>); }
    if (name == "sym2"sv) { coeffs = std::data(sym2<float>); }
    if (name == "sym3"sv) { coeffs = std::data(sym3<float>); }
    if (name == "sym4"sv) { coeffs = std::data(sym4<float>); }
    if (name == "sym5"sv) { coeffs = std::data(sym5<float>); }
    if (name == "sym6"sv) { coeffs = std::data(sym6<float>); }
    if (name == "sym7"sv) { coeffs = std::data(sym7<float>); }
    if (name == "sym8"sv) { coeffs = std::data(sym8<float>); }
    if (name == "sym9"sv) { coeffs = std::data(sym9<float>); }
    if (name == "sym10"sv) { coeffs = std::data(sym10<float>); }
    if (name == "sym11"sv) { coeffs = std::data(sym11<float>); }
    if (name == "sym12"sv) { coeffs = std::data(sym12<float>); }
    if (name == "sym13"sv) { coeffs = std::data(sym13<float>); }
    if (name == "sym14"sv) { coeffs = std::data(sym14<float>); }
    if (name == "sym15"sv) { coeffs = std::data(sym15<float>); }
    if (name == "sym16"sv) { coeffs = std::data(sym16<float>); }
    if (name == "sym17"sv) { coeffs = std::data(sym17<float>); }
    if (name == "sym18"sv) { coeffs = std::data(sym18<float>); }
    if (name == "sym19"sv) { coeffs = std::data(sym19<float>); }
    if (name == "sym20"sv) { coeffs = std::data(sym20<float>); }
    if (name == "meyer"sv) { coeffs = std::data(meyer<float>); }

    auto const n = waveletFilterLength(name);
    std::reverse_copy(coeffs, coeffs + n, lp1);
    qmfWrev(coeffs, n, hp1);
    std::copy(coeffs, std::next(coeffs, n), lp2);
    qmfEven(coeffs, n, hp2);
    return n;
}

auto waveletFilterCoefficients(
    char const* name,
    float* lp1,
    float* hp1,
    float* lp2,
    float* hp2
) -> int
{
    auto const n        = waveletFilterLength(name);
    auto const nameView = StringView{name};
    if (nameView.find("haar") != StringView::npos) {
        return fillDauberchies(name, lp1, hp1, lp2, hp2);
    }
    if (nameView.find("db") != StringView::npos) {
        return fillDauberchies(name, lp1, hp1, lp2, hp2);
    }
    if (nameView.find("sym") != StringView::npos) {
        return fillDauberchies(name, lp1, hp1, lp2, hp2);
    }
    if (nameView.find("meyer") != StringView::npos) {
        return fillDauberchies(name, lp1, hp1, lp2, hp2);
    }

    if (name == StringView{"coif1"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif1<float>), cend(coif1<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    if (name == StringView{"coif2"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif2<float>), cend(coif2<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    if (name == StringView{"coif3"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif3<float>), cend(coif3<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    if (name == StringView{"coif4"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif4<float>), cend(coif4<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    if (name == StringView{"coif5"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif5<float>), cend(coif5<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    if (name == StringView{"coif6"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif6<float>), cend(coif6<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    if (name == StringView{"coif7"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif7<float>), cend(coif7<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    if (name == StringView{"coif8"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif8<float>), cend(coif8<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    if (name == StringView{"coif9"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif9<float>), cend(coif9<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    if (name == StringView{"coif10"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif10<float>), cend(coif10<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }
    if (name == StringView{"coif11"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif11<float>), cend(coif11<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }
    if (name == StringView{"coif12"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif12<float>), cend(coif12<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }
    if (name == StringView{"coif13"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif13<float>), cend(coif13<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }
    if (name == StringView{"coif14"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif14<float>), cend(coif14<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }
    if (name == StringView{"coif15"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif15<float>), cend(coif15<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }
    if (name == StringView{"coif16"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif16<float>), cend(coif16<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }
    if (name == StringView{"coif17"}) {
        auto coeffTemp = makeUnique<float[]>(n);

        std::copy(cbegin(coif17<float>), cend(coif17<float>), coeffTemp.get());
        for (auto i = 0; i < n; ++i) { coeffTemp[i] *= static_cast<float>(numbers::sqrt2); }

        std::reverse_copy(coeffTemp.get(), coeffTemp.get() + n, lp1);
        qmfWrev(coeffTemp.get(), n, hp1);
        std::copy(coeffTemp.get(), coeffTemp.get() + n, lp2);
        qmfEven(coeffTemp.get(), n, hp2);

        return n;
    }

    print("\n Filter Not in Database \n");
    return -1;
}

}  // namespace

Wavelet::Wavelet(char const* name)
    : _name{name}
    , _size{static_cast<std::size_t>(waveletFilterLength(name))}
    , _params{makeUnique<float[]>(4 * _size)}
{
    waveletFilterCoefficients(name, data(lpd()), data(hpd()), data(lpr()), data(hpr()));
}

auto Wavelet::size() const noexcept -> std::size_t { return _size; }

auto Wavelet::name() const noexcept -> String const& { return _name; }

auto Wavelet::lpd() const noexcept -> Span<float> { return {&_params[0], size()}; }

auto Wavelet::hpd() const noexcept -> Span<float> { return {&_params[size()], size()}; }

auto Wavelet::lpr() const noexcept -> Span<float>
{
    return {&_params[size() * 2U], size()};
}

auto Wavelet::hpr() const noexcept -> Span<float>
{
    return {&_params[size() * 3U], size()};
}

auto summary(Wavelet const& wavelet) -> String
{
    auto s = String{};
    fmt::format_to(std::back_inserter(s), "Wavelet: {0}\n", wavelet.name());
    fmt::format_to(std::back_inserter(s), "  Filters length: {0}\n", wavelet.size());

    fmt::format_to(std::back_inserter(s), "lpd: [{0}]\n", fmt::join(wavelet.lpd(), ", "));
    fmt::format_to(std::back_inserter(s), "hpd: [{0}]\n", fmt::join(wavelet.hpd(), ", "));
    fmt::format_to(std::back_inserter(s), "lpr: [{0}]\n", fmt::join(wavelet.lpr(), ", "));
    fmt::format_to(std::back_inserter(s), "hpr: [{0}]\n", fmt::join(wavelet.hpr(), ", "));
    return s;
}

}  // namespace mc::dsp
