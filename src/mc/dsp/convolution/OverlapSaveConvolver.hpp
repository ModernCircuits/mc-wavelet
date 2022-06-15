#pragma once

#include "mc/algorithm.hpp"
#include "mc/array.hpp"
#include "mc/cmath.hpp"
#include "mc/complex.hpp"
#include "mc/cstring.hpp"
#include "mc/iterator.hpp"
#include "mc/memory.hpp"
#include "mc/preprocessor.hpp"
#include "mc/vector.hpp"

#include "fftw3.h"

namespace mc::dsp
{

/// This class is a Signal that works on aligned float arrays allocated by FFTW.
/// It also overloads some further operators to do basic arithmetic
struct FloatSignal
{
    /// the basic constructor allocates an aligned, float array, which is zeroed by the superclass
    explicit FloatSignal(std::size_t size);
    FloatSignal(float* data, size_t size);
    FloatSignal(float* data, size_t size, size_t padBef, size_t padAft);
    ~FloatSignal();

    [[nodiscard]] auto begin() -> float* { return data_; }
    [[nodiscard]] auto begin() const -> float const* { return data_; }
    [[nodiscard]] auto cbegin() const -> float const* { return data_; }

    [[nodiscard]] auto end() -> float* { return begin() + size(); }
    [[nodiscard]] auto end() const -> float const* { return begin() + size(); }
    [[nodiscard]] auto cend() const -> float const* { return begin() + size(); }

    [[nodiscard]] auto size() -> size_t& { return size_; }
    [[nodiscard]] auto size() const -> std::size_t const& { return size_; }
    [[nodiscard]] auto data() -> float* { return data_; }
    [[nodiscard]] auto data() const -> float const* { return data_; }

    [[nodiscard]] auto operator[](std::size_t idx) -> float& { return data_[idx]; }
    [[nodiscard]] auto operator[](std::size_t idx) const -> float& { return data_[idx]; }

protected:
    float* data_;
    std::size_t size_;
};

/// This class is a Signal that works on aligned complex (float[2]) arrays allocated by FFTW.
/// It also overloads some further operators to do basic arithmetic
struct ComplexSignal
{
    /// the basic constructor allocates an aligned, float[2] array, which is zeroed by the superclass
    explicit ComplexSignal(std::size_t size);
    ~ComplexSignal();

    [[nodiscard]] auto begin() -> Complex<float>* { return data_; }
    [[nodiscard]] auto begin() const -> Complex<float> const* { return data_; }
    [[nodiscard]] auto cbegin() const -> Complex<float> const* { return data_; }

    [[nodiscard]] auto end() -> Complex<float>* { return begin() + size(); }
    [[nodiscard]] auto end() const -> Complex<float> const* { return begin() + size(); }
    [[nodiscard]] auto cend() const -> Complex<float> const* { return begin() + size(); }

    [[nodiscard]] auto size() -> size_t& { return size_; }
    [[nodiscard]] auto size() const -> std::size_t const& { return size_; }
    [[nodiscard]] auto data() -> Complex<float>* { return data_; }
    [[nodiscard]] auto data() const -> Complex<float> const* { return data_; }

    [[nodiscard]] auto operator[](std::size_t idx) -> Complex<float>& { return data_[idx]; }
    [[nodiscard]] auto operator[](std::size_t idx) const -> Complex<float>& { return data_[idx]; }

protected:
    Complex<float>* data_;
    std::size_t size_;
};

/// This free function takes three complex signals a,b,c of the same size and computes the complex
/// element-wise multiplication:   a+ib * c+id = ac+iad+ibc-bd = ac-bd + i(ad+bc)   The computation
/// loop isn't sent to OMP because this function itself is already expected to be called by multiple
/// threads, and it would actually slow down the process.
/// It throuws an exception if
auto spectralConvolution(ComplexSignal const& a, ComplexSignal const& b, ComplexSignal& result) -> void;

/// This function behaves identically to SpectralConvolution, but computes c=a*conj(b) instead
/// of c=a*b:         a * conj(b) = a+ib * c-id = ac-iad+ibc+bd = ac+bd + i(bc-ad)
auto spectralCorrelation(ComplexSignal const& a, ComplexSignal const& b, ComplexSignal& result) -> void;

/// This class is a simple wrapper for the memory management of the fftw plans, plus a
/// parameterless execute() method which is also a wrapper for FFTW's execute.
/// It is not expected to be used directly: rather, to be extended by specific plans, for instance,
/// if working with real, 1D signals, only 1D complex<->real plans are needed.
struct FftPlan
{
    explicit FftPlan(fftwf_plan p) : plan_(p) {}
    ~FftPlan() { fftwf_destroy_plan(plan_); }
    auto execute() { fftwf_execute(plan_); }

private:
    fftwf_plan plan_;
};

// This forward plan (1D, R->C) is adequate to process 1D floats (real).
struct FftForwardPlan : FftPlan
{
    // This constructor creates a real->complex plan that performs the FFT(real) and saves it into the
    // complex. As explained in the FFTW docs (http://www.fftw.org/#documentation), the size of
    // the complex has to be size(real)/2+1, so the constructor will throw a runtime error if
    // this condition doesn't hold. Since the signals and the superclass already have proper
    // destructors, no special memory management has to be done.
    explicit FftForwardPlan(FloatSignal& fs, ComplexSignal& cs);
};

// This backward plan (1D, C->R) is adequate to process spectra of 1D floats (real).
struct FftBackwardPlan : FftPlan
{
    // This constructor creates a complex->real plan that performs the IFFT(complex) and saves it
    // complex. As explained in the FFTW docs (http://www.fftw.org/#documentation), the size of
    // the complex has to be size(real)/2+1, so the constructor will throw a runtime error if
    // this condition doesn't hold. Since the signals and the superclass already have proper
    // destructors, no special memory management has to be done.
    explicit FftBackwardPlan(ComplexSignal& cs, FloatSignal& fs);
};

/// This class performs an efficient version of the spectral convolution/cross-correlation between
/// two 1D float arrays, <SIGNAL> and <PATCH>, called overlap-save:
/// http://www.comm.utoronto.ca/~dkundur/course_info/real-time-DSP/notes/8_Kundur_Overlap_Save_Add.pdf
/// This algorithm requires that the length of <PATCH> is less or equal the length of <SIGNAL>,
/// so an exception is thrown otherwise. The algorithm works as follows:
/// given signal of length S and patch of length P, and being the conv (or xcorr) length U=S+P-1
///   1. pad the patch to X = 2*Pow2Ceil(P). FFTs with powers of 2 are the fastest.
///   2. cut the signal into chunks of size X, with an overlapping section of L=X-(P-1).
///      for that, pad the signal with (P-1) before, and with (X-U%L) after, to make it fit exactly.
///   3. Compute the forward FFT of the padded patch and of every chunk of the signal
///   4. Multiply the FFT of the padded patch with every signal chunk.
///      4a. If the operation is a convolution, perform a complex a*b multiplication
///      4b. If the operation is a cross-correlation, perform a complex a*conj(b) multiplication
///   5. Compute the inverse FFT of every result of step 4
///   6. Concatenate the resulting chunks, ignoring (P-1) samples per chunk
/// Note that steps 3,4,5 may be parallelized with some significant gain in performance.
/// In this class: X = result_chunksize, L = result_stride
struct OverlapSaveConvolver
{
    /// The only constructor for the class, receives two signals and performs steps 1 and 2 of the
    /// algorithm on them. The signals are passed by reference but the class works with padded copies
    /// of them, so no care has to be taken regarding memory management.
    /// The wisdomPath may be empty, or a path to a valid wisdom file.
    /// Note that len(signal) can never be smaller than len(patch), or an exception is thrown.
    OverlapSaveConvolver(FloatSignal& signal, FloatSignal& patch, std::string const& wisdomPath = "");

    auto convolute() -> void;
    auto crossCorrelate() -> void;

    // This method implements step 6 of the overlap-save algorithm. In convolution, the first (P-1)
    // samples of each chunk are discarded, in xcorr the last (P-1) ones. Therefore, depending on the
    // current _state_, the corresponding method is used. USAGE:
    // Every time it is called, this function returns a new FloatSignal instance of size
    // len(signal)+len(patch)-1. If the last operation performed was convolute(), this function
    // will return the  convolution of signal and patch. If the last operation performed was
    // crossCorrelate(), the result will contain the cross-correlation. If none of them was performed
    // at the moment of calling this function, an exception will be thrown.
    // The indexing will start with the most negative relation, and increase accordingly. Which means:
    //   given S:=len(signal), P:=len(patch), T:=S+P-1
    // for 0 <= i < T, result[i] will hold dot_product(patch, signal[i-(P-1) : i])
    //   where patch will be "reversed" if the convolution was performed. For example:
    // Signal :=        [1 2 3 4 5 6 7]    Patch = [1 1 1]
    // Result[0] =  [1 1 1]                        => 1*1         = 1  // FIRST ENTRY
    // Result[1] =    [1 1 1]                      => 1*1+1*2     = 3
    // Result[2] =      [1 1 1]                    => 1*1+1*2+1*3 = 8  // FIRST NON-NEG ENTRY AT P-1
    //   ...
    // Result[8] =                  [1 1 1]        => 1*7         = 7  // LAST ENTRY
    // Note that the returned signal object takes care of its own memory, so no management is needed.
    auto extractResult() -> FloatSignal;

private:
    // This private method implements steps 3,4,5 of the algorithm. If the given flag is false,
    // it will perform a convolution (4a), and a cross-correlation (4b) otherwise.
    // Note the parallelization with OpenMP, which increases performance in supporting CPUs.
    auto execute(bool crossCorrelate) -> void;

    // grab input lengths
    std::size_t signalSize_;
    std::size_t patchSize_;
    std::size_t resultSize_;

    // make padded copies of the inputs and get chunk measurements
    FloatSignal paddedPatch_;
    std::size_t resultChunksize_;
    std::size_t resultChunksizeComplex_;
    std::size_t result_stride_;
    ComplexSignal paddedPatchComplex_;

    // padded copy of the signal
    FloatSignal paddedSignal_;

    // the deconstructed signal
    std::vector<std::unique_ptr<FloatSignal>> inputChunks_;
    std::vector<std::unique_ptr<ComplexSignal>> inputChunksComplex_;

    // the corresponding chunks holding convs/xcorrs
    std::vector<std::unique_ptr<FloatSignal>> resultChunks_;
    std::vector<std::unique_ptr<ComplexSignal>> resultChunksComplex_;

    // the corresponding plans (plus the plan of the patch)
    std::vector<std::unique_ptr<FftForwardPlan>> forwardPlans_;
    std::vector<std::unique_ptr<FftBackwardPlan>> backwardPlans_;

    // Basic state management to prevent getters from being called prematurely.
    // Also to adapt the extractResult getter, since Conv and Xcorr padding behaves differently
    enum class State
    {
        Uninitialized,
        Conv,
        Xcorr
    };

    State state_{State::Uninitialized};  // Uninitialized after instantiation, Conv/Xcorr after respective op.
};
}  // namespace mc::dsp