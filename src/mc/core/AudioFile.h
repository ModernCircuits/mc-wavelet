/** @file AudioFile.h
 *  @author Adam Stark
 *  @copyright Copyright (C) 2017  Adam Stark
 *
 * This file is part of the 'AudioFile' library
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AS_AudioFile_h
#define AS_AudioFile_h

#include <mc/core/algorithm.hpp>
#include <mc/core/cassert.hpp>
#include <mc/core/config.hpp>
#include <mc/core/cstring.hpp>
#include <mc/core/format.hpp>
#include <mc/core/fstream.hpp>
#include <mc/core/iterator.hpp>
#include <mc/core/limits.hpp>
#include <mc/core/string.hpp>
#include <mc/core/unordered_map.hpp>
#include <mc/core/vector.hpp>

// disable some warnings on Windows
#if defined(_MSC_VER)
__pragma(warning(push)) __pragma(warning(disable : 4244)) __pragma(warning(disable : 4457))
    __pragma(warning(disable : 4458)) __pragma(warning(disable : 4389)) __pragma(warning(disable : 4996))
#elif defined(__GNUC__)
_Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wconversion\"")
    _Pragma("GCC diagnostic ignored \"-Wsign-compare\"") _Pragma("GCC diagnostic ignored \"-Wshadow\"")
#endif

    /** The different types of audio file, plus some other types to
     * indicate a failure to load a file, or that one hasn't been
     * loaded yet
     */
    enum class AudioFileFormat {
        Error,
        NotLoaded,
        Wave,
        Aiff
    };

template<class T>
class AudioFile
{
public:
    using AudioBuffer = std::vector<std::vector<T>>;

    /** Constructor */
    AudioFile();

    /** Constructor, using a given file path to load a file */
    explicit AudioFile(std::string filePath);

    /** Loads an audio file from a given file path.
     * @Returns true if the file was successfully loaded
     */
    auto load(std::string const& filePath) -> bool;

    /** Saves an audio file to a given file path.
     * @Returns true if the file was successfully saved
     */
    auto save(std::string filePath, AudioFileFormat format = AudioFileFormat::Wave) -> bool;

    /** @Returns the sample rate */
    [[nodiscard]] auto getSampleRate() const -> uint32_t;

    /** @Returns the number of audio channels in the buffer */
    [[nodiscard]] auto getNumChannels() const -> int;

    /** @Returns true if the audio file is mono */
    [[nodiscard]] auto isMono() const -> bool;

    /** @Returns true if the audio file is stereo */
    [[nodiscard]] auto isStereo() const -> bool;

    /** @Returns the bit depth of each sample */
    [[nodiscard]] auto getBitDepth() const -> int;

    /** @Returns the number of samples per channel */
    [[nodiscard]] auto getNumSamplesPerChannel() const -> int;

    /** @Returns the length in seconds of the audio file based on the number of samples and sample rate */
    [[nodiscard]] auto getLengthInSeconds() const -> float;

    /** Prints a summary of the audio file to the console */
    void printSummary() const;

    /** Set the audio buffer for this AudioFile by copying samples from another buffer.
     * @Returns true if the buffer was copied successfully.
     */
    auto setAudioBuffer(AudioBuffer& newBuffer) -> bool;

    /** Sets the audio buffer to a given number of channels and number of samples per channel. This will try to preserve
     * the existing audio, adding zeros to any new channels or new samples in a given channel.
     */
    void setAudioBufferSize(int numChannels, int numSamples);

    /** Sets the number of samples per channel in the audio buffer. This will try to preserve
     * the existing audio, adding zeros to new samples in a given channel if the number of samples is increased.
     */
    void setNumSamplesPerChannel(int numSamples);

    /** Sets the number of channels. New channels will have the correct number of samples and be initialised to zero */
    void setNumChannels(int numChannels);

    /** Sets the bit depth for the audio file. If you use the save() function, this bit depth rate will be used */
    void setBitDepth(int numBitsPerSample);

    /** Sets the sample rate for the audio file. If you use the save() function, this sample rate will be used */
    void setSampleRate(uint32_t newSampleRate);

    /** Sets whether the library should log error messages to the console. By default this is true */
    void shouldLogErrorsToConsole(bool logErrors);

    /** A vector of vectors holding the audio samples for the AudioFile. You can
     * access the samples by channel and then by sample index, i.e:
     *
     *      samples[channel][sampleIndex]
     */
    AudioBuffer samples;

    /** An optional iXML chunk that can be added to the AudioFile.
     */
    std::string iXMLChunk;

private:
    enum class Endianness
    {
        LittleEndian,
        BigEndian
    };

    auto determineAudioFileFormat(std::vector<uint8_t>& fileData) -> AudioFileFormat;
    auto decodeWaveFile(std::vector<uint8_t>& fileData) -> bool;
    auto decodeAiffFile(std::vector<uint8_t>& fileData) -> bool;

    auto saveToWaveFile(std::string filePath) -> bool;
    auto saveToAiffFile(std::string filePath) -> bool;

    void clearAudioBuffer();

    auto fourBytesToInt(std::vector<uint8_t>& source, int startIndex, Endianness endianness = Endianness::LittleEndian)
        -> int32_t;
    auto twoBytesToInt(std::vector<uint8_t>& source, int startIndex, Endianness endianness = Endianness::LittleEndian)
        -> int16_t;
    auto getIndexOfString(std::vector<uint8_t>& source, std::string const& s) -> int;
    auto getIndexOfChunk(std::vector<uint8_t>& source, std::string const& chunkHeaderID, int startIndex,
                         Endianness endianness = Endianness::LittleEndian) -> int;

    auto sixteenBitIntToSample(int16_t sample) -> T;
    auto sampleToSixteenBitInt(T sample) -> int16_t;

    auto sampleToSingleByte(T sample) -> uint8_t;
    auto singleByteToSample(uint8_t sample) -> T;

    auto getAiffSampleRate(std::vector<uint8_t>& fileData, int sampleRateStartIndex) -> uint32_t;
    auto tenByteMatch(std::vector<uint8_t>& v1, int startIndex1, std::vector<uint8_t>& v2, int startIndex2) -> bool;
    void addSampleRateToAiffData(std::vector<uint8_t>& fileData, uint32_t sampleRate);
    auto clamp(T value, T minValue, T maxValue) -> T;

    void addStringToFileData(std::vector<uint8_t>& fileData, std::string const& s);
    void addInt32ToFileData(std::vector<uint8_t>& fileData, int32_t i,
                            Endianness endianness = Endianness::LittleEndian);
    void addInt16ToFileData(std::vector<uint8_t>& fileData, int16_t i,
                            Endianness endianness = Endianness::LittleEndian);

    auto writeDataToFile(std::vector<uint8_t>& fileData, std::string const& filePath) -> bool;

    void reportError(std::string const& errorMessage);

    AudioFileFormat audioFileFormat_;
    uint32_t sampleRate_{};
    int bitDepth_{};
    bool logErrorsToConsole_{true};
};

// Pre-defined 10-byte representations of common sample rates
static std::unordered_map<uint32_t, std::vector<uint8_t>> aiffSampleRateTable
    = {{8000, {64, 11, 250, 0, 0, 0, 0, 0, 0, 0}},    {11025, {64, 12, 172, 68, 0, 0, 0, 0, 0, 0}},
       {16000, {64, 12, 250, 0, 0, 0, 0, 0, 0, 0}},   {22050, {64, 13, 172, 68, 0, 0, 0, 0, 0, 0}},
       {32000, {64, 13, 250, 0, 0, 0, 0, 0, 0, 0}},   {37800, {64, 14, 147, 168, 0, 0, 0, 0, 0, 0}},
       {44056, {64, 14, 172, 24, 0, 0, 0, 0, 0, 0}},  {44100, {64, 14, 172, 68, 0, 0, 0, 0, 0, 0}},
       {47250, {64, 14, 184, 146, 0, 0, 0, 0, 0, 0}}, {48000, {64, 14, 187, 128, 0, 0, 0, 0, 0, 0}},
       {50000, {64, 14, 195, 80, 0, 0, 0, 0, 0, 0}},  {50400, {64, 14, 196, 224, 0, 0, 0, 0, 0, 0}},
       {88200, {64, 15, 172, 68, 0, 0, 0, 0, 0, 0}},  {96000, {64, 15, 187, 128, 0, 0, 0, 0, 0, 0}},
       {176400, {64, 16, 172, 68, 0, 0, 0, 0, 0, 0}}, {192000, {64, 16, 187, 128, 0, 0, 0, 0, 0, 0}},
       {352800, {64, 17, 172, 68, 0, 0, 0, 0, 0, 0}}, {2822400, {64, 20, 172, 68, 0, 0, 0, 0, 0, 0}},
       {5644800, {64, 21, 172, 68, 0, 0, 0, 0, 0, 0}}};

enum WavAudioFormat
{
    PCM        = 0x0001,
    IEEEFloat  = 0x0003,
    ALaw       = 0x0006,
    MULaw      = 0x0007,
    Extensible = 0xFFFE
};

enum AIFFAudioFormat
{
    Uncompressed,
    Compressed,
    Error
};

/* IMPLEMENTATION */

template<class T>
AudioFile<T>::AudioFile()
{
    static_assert(std::is_floating_point<T>::value,
                  "ERROR: This version of AudioFile only supports floating point sample formats");

    bitDepth_   = 16;
    sampleRate_ = 44100;
    samples.resize(1);
    samples[0].resize(0);
    audioFileFormat_ = AudioFileFormat::NotLoaded;
}

template<class T>
AudioFile<T>::AudioFile(std::string filePath) : AudioFile<T>()
{
    load(filePath);
}

template<class T>
auto AudioFile<T>::getSampleRate() const -> uint32_t
{
    return sampleRate_;
}

template<class T>
auto AudioFile<T>::getNumChannels() const -> int
{
    return (int)samples.size();
}

template<class T>
auto AudioFile<T>::isMono() const -> bool
{
    return getNumChannels() == 1;
}

template<class T>
auto AudioFile<T>::isStereo() const -> bool
{
    return getNumChannels() == 2;
}

template<class T>
auto AudioFile<T>::getBitDepth() const -> int
{
    return bitDepth_;
}

template<class T>
auto AudioFile<T>::getNumSamplesPerChannel() const -> int
{
    if (!samples.empty()) { return (int)samples[0].size(); }
    return 0;
}

template<class T>
auto AudioFile<T>::getLengthInSeconds() const -> float
{
    return (float)getNumSamplesPerChannel() / (float)sampleRate_;
}

template<class T>
void AudioFile<T>::printSummary() const
{
    fmt::print("|======================================|\n");
    fmt::print("Num Channels: {}\n", getNumChannels());
    fmt::print("Num Samples Per Channel: {0}\n", getNumSamplesPerChannel());
    fmt::print("Sample Rate: {0}\n", sampleRate_);
    fmt::print("Bit Depth: {0}\n", bitDepth_);
    fmt::print("Length in Seconds: {0}\n", getLengthInSeconds());
    fmt::print("|======================================|\n");
}

template<class T>
auto AudioFile<T>::setAudioBuffer(AudioBuffer& newBuffer) -> bool
{
    int numChannels = (int)newBuffer.size();

    if (numChannels <= 0)
    {
        MC_ASSERT(false && "The buffer your are trying to use has no channels");
        return false;
    }

    size_t numSamples = newBuffer[0].size();

    // set the number of channels
    samples.resize(newBuffer.size());

    for (int k = 0; k < getNumChannels(); k++)
    {
        MC_ASSERT(newBuffer[k].size() == numSamples);

        samples[k].resize(numSamples);

        for (size_t i = 0; i < numSamples; i++) { samples[k][i] = newBuffer[k][i]; }
    }

    return true;
}

template<class T>
void AudioFile<T>::setAudioBufferSize(int numChannels, int numSamples)
{
    samples.resize(numChannels);
    setNumSamplesPerChannel(numSamples);
}

template<class T>
void AudioFile<T>::setNumSamplesPerChannel(int numSamples)
{
    int originalSize = getNumSamplesPerChannel();

    for (int i = 0; i < getNumChannels(); i++)
    {
        samples[i].resize(numSamples);

        // set any new samples to zero
        if (numSamples > originalSize) { std::fill(samples[i].begin() + originalSize, samples[i].end(), (T)0.); }
    }
}

template<class T>
void AudioFile<T>::setNumChannels(int numChannels)
{
    int originalNumChannels          = getNumChannels();
    int originalNumSamplesPerChannel = getNumSamplesPerChannel();

    samples.resize(numChannels);

    // make sure any new channels are set to the right size
    // and filled with zeros
    if (numChannels > originalNumChannels)
    {
        for (int i = originalNumChannels; i < numChannels; i++)
        {
            samples[i].resize(originalNumSamplesPerChannel);
            std::fill(samples[i].begin(), samples[i].end(), (T)0.);
        }
    }
}

template<class T>
void AudioFile<T>::setBitDepth(int numBitsPerSample)
{
    bitDepth_ = numBitsPerSample;
}

template<class T>
void AudioFile<T>::setSampleRate(uint32_t newSampleRate)
{
    sampleRate_ = newSampleRate;
}

template<class T>
void AudioFile<T>::shouldLogErrorsToConsole(bool logErrors)
{
    logErrorsToConsole_ = logErrors;
}

template<class T>
auto AudioFile<T>::load(std::string const& filePath) -> bool
{
    std::ifstream file(filePath, std::ios::binary);

    // check the file exists
    if (!file.good())
    {
        reportError("ERROR: File doesn't exist or otherwise can't load file\n" + filePath);
        return false;
    }

    std::vector<uint8_t> fileData;

    file.unsetf(std::ios::skipws);

    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    file.seekg(0, std::ios::beg);

    // allocate
    fileData.resize(length);

    file.read(reinterpret_cast<char*>(fileData.data()), length);
    file.close();

    if (file.gcount() != length)
    {
        reportError("ERROR: Couldn't read entire file\n" + filePath);
        return false;
    }

    // get audio file format
    audioFileFormat_ = determineAudioFileFormat(fileData);

    if (audioFileFormat_ == AudioFileFormat::Wave) { return decodeWaveFile(fileData); }
    if (audioFileFormat_ == AudioFileFormat::Aiff) { return decodeAiffFile(fileData); }
    reportError("Audio File Type: Error");
    return false;
}

template<class T>
auto AudioFile<T>::decodeWaveFile(std::vector<uint8_t>& fileData) -> bool
{
    // -----------------------------------------------------------
    // HEADER CHUNK
    std::string headerChunkID(fileData.begin(), fileData.begin() + 4);
    // int32_t fileSizeInBytes = fourBytesToInt (fileData, 4) + 8;
    std::string format(fileData.begin() + 8, fileData.begin() + 12);

    // -----------------------------------------------------------
    // try and find the start points of key chunks
    int indexOfDataChunk   = getIndexOfChunk(fileData, "data", 12);
    int indexOfFormatChunk = getIndexOfChunk(fileData, "fmt ", 12);
    int indexOfXMLChunk    = getIndexOfChunk(fileData, "iXML", 12);

    // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
    // then it is unlikely we'll able to read this file, so abort
    if (indexOfDataChunk == -1 || indexOfFormatChunk == -1 || headerChunkID != "RIFF" || format != "WAVE")
    {
        reportError("ERROR: this doesn't seem to be a valid .WAV file");
        return false;
    }

    // -----------------------------------------------------------
    // FORMAT CHUNK
    int f = indexOfFormatChunk;
    std::string formatChunkID(fileData.begin() + f, fileData.begin() + f + 4);
    // int32_t formatChunkSize = fourBytesToInt (fileData, f + 4);
    uint16_t audioFormat       = twoBytesToInt(fileData, f + 8);
    uint16_t numChannels       = twoBytesToInt(fileData, f + 10);
    sampleRate_                = (uint32_t)fourBytesToInt(fileData, f + 12);
    uint32_t numBytesPerSecond = fourBytesToInt(fileData, f + 16);
    uint16_t numBytesPerBlock  = twoBytesToInt(fileData, f + 20);
    bitDepth_                  = (int)twoBytesToInt(fileData, f + 22);

    uint16_t numBytesPerSample = static_cast<uint16_t>(bitDepth_) / 8;

    // check that the audio format is PCM or Float or extensible
    if (audioFormat != WavAudioFormat::PCM && audioFormat != WavAudioFormat::IEEEFloat
        && audioFormat != WavAudioFormat::Extensible)
    {
        reportError("ERROR: this .WAV file is encoded in a format that this library does not support at present");
        return false;
    }

    // check the number of channels is mono or stereo
    if (numChannels < 1 || numChannels > 128)
    {
        reportError("ERROR: this WAV file seems to be an invalid number of channels (or corrupted?)");
        return false;
    }

    // check header data is consistent
    if (numBytesPerSecond != static_cast<uint32_t>((numChannels * sampleRate_ * bitDepth_) / 8)
        || numBytesPerBlock != (numChannels * numBytesPerSample))
    {
        reportError("ERROR: the header data in this WAV file seems to be inconsistent");
        return false;
    }

    // check bit depth is either 8, 16, 24 or 32 bit
    if (bitDepth_ != 8 && bitDepth_ != 16 && bitDepth_ != 24 && bitDepth_ != 32)
    {
        reportError("ERROR: this file has a bit depth that is not 8, 16, 24 or 32 bits");
        return false;
    }

    // -----------------------------------------------------------
    // DATA CHUNK
    int d = indexOfDataChunk;
    std::string dataChunkID(fileData.begin() + d, fileData.begin() + d + 4);
    int32_t dataChunkSize = fourBytesToInt(fileData, d + 4);

    int numSamples        = dataChunkSize / (numChannels * bitDepth_ / 8);
    int samplesStartIndex = indexOfDataChunk + 8;

    clearAudioBuffer();
    samples.resize(numChannels);

    for (int i = 0; i < numSamples; i++)
    {
        for (int channel = 0; channel < numChannels; channel++)
        {
            int sampleIndex = samplesStartIndex + (numBytesPerBlock * i) + channel * numBytesPerSample;

            if ((sampleIndex + (bitDepth_ / 8) - 1) >= fileData.size())
            {
                reportError(
                    "ERROR: read file error as the metadata indicates more samples than there are in the file data");
                return false;
            }

            if (bitDepth_ == 8)
            {
                T sample = singleByteToSample(fileData[sampleIndex]);
                samples[channel].push_back(sample);
            }
            else if (bitDepth_ == 16)
            {
                int16_t sampleAsInt = twoBytesToInt(fileData, sampleIndex);
                T sample            = sixteenBitIntToSample(sampleAsInt);
                samples[channel].push_back(sample);
            }
            else if (bitDepth_ == 24)
            {
                int32_t sampleAsInt = 0;
                sampleAsInt
                    = (fileData[sampleIndex + 2] << 16) | (fileData[sampleIndex + 1] << 8) | fileData[sampleIndex];

                if ((sampleAsInt & 0x800000) != 0)
                {  //  if the 24th bit is set, this is a negative number in 24-bit world
                    sampleAsInt = sampleAsInt | ~0xFFFFFF;  // so make sure sign is extended to the 32 bit float
                }

                T sample = (T)sampleAsInt / (T)8388608.;
                samples[channel].push_back(sample);
            }
            else if (bitDepth_ == 32)
            {
                int32_t sampleAsInt = fourBytesToInt(fileData, sampleIndex);
                T sample;

                if (audioFormat == WavAudioFormat::IEEEFloat) { sample = (T) reinterpret_cast<float&>(sampleAsInt); }
                else
                {  // assume PCM
                    sample = (T)sampleAsInt / static_cast<float>(std::numeric_limits<std::int32_t>::max());
                }

                samples[channel].push_back(sample);
            }
            else { MC_ASSERT(false); }
        }
    }

    // -----------------------------------------------------------
    // iXML CHUNK
    if (indexOfXMLChunk != -1)
    {
        int32_t chunkSize = fourBytesToInt(fileData, indexOfXMLChunk + 4);
        iXMLChunk         = std::string((char const*)&fileData[indexOfXMLChunk + 8], chunkSize);
    }

    return true;
}

template<class T>
auto AudioFile<T>::decodeAiffFile(std::vector<uint8_t>& fileData) -> bool
{
    // -----------------------------------------------------------
    // HEADER CHUNK
    std::string headerChunkID(fileData.begin(), fileData.begin() + 4);
    // int32_t fileSizeInBytes = fourBytesToInt (fileData, 4, Endianness::BigEndian) + 8;
    std::string format(fileData.begin() + 8, fileData.begin() + 12);

    int audioFormat = format == "AIFF"   ? AIFFAudioFormat::Uncompressed
                      : format == "AIFC" ? AIFFAudioFormat::Compressed
                                         : AIFFAudioFormat::Error;

    // -----------------------------------------------------------
    // try and find the start points of key chunks
    int indexOfCommChunk      = getIndexOfChunk(fileData, "COMM", 12, Endianness::BigEndian);
    int indexOfSoundDataChunk = getIndexOfChunk(fileData, "SSND", 12, Endianness::BigEndian);
    int indexOfXMLChunk       = getIndexOfChunk(fileData, "iXML", 12, Endianness::BigEndian);

    // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
    // then it is unlikely we'll able to read this file, so abort
    if (indexOfSoundDataChunk == -1 || indexOfCommChunk == -1 || headerChunkID != "FORM"
        || audioFormat == AIFFAudioFormat::Error)
    {
        reportError("ERROR: this doesn't seem to be a valid AIFF file");
        return false;
    }

    // -----------------------------------------------------------
    // COMM CHUNK
    int p = indexOfCommChunk;
    std::string commChunkID(fileData.begin() + p, fileData.begin() + p + 4);
    // int32_t commChunkSize = fourBytesToInt (fileData, p + 4, Endianness::BigEndian);
    int16_t numChannels          = twoBytesToInt(fileData, p + 8, Endianness::BigEndian);
    int32_t numSamplesPerChannel = fourBytesToInt(fileData, p + 10, Endianness::BigEndian);
    bitDepth_                    = (int)twoBytesToInt(fileData, p + 14, Endianness::BigEndian);
    sampleRate_                  = getAiffSampleRate(fileData, p + 16);

    // check the sample rate was properly decoded
    if (sampleRate_ == 0)
    {
        reportError("ERROR: this AIFF file has an unsupported sample rate");
        return false;
    }

    // check the number of channels is mono or stereo
    if (numChannels < 1 || numChannels > 2)
    {
        reportError("ERROR: this AIFF file seems to be neither mono nor stereo (perhaps multi-track, or corrupted?)");
        return false;
    }

    // check bit depth is either 8, 16, 24 or 32-bit
    if (bitDepth_ != 8 && bitDepth_ != 16 && bitDepth_ != 24 && bitDepth_ != 32)
    {
        reportError("ERROR: this file has a bit depth that is not 8, 16, 24 or 32 bits");
        return false;
    }

    // -----------------------------------------------------------
    // SSND CHUNK
    int s = indexOfSoundDataChunk;
    std::string soundDataChunkID(fileData.begin() + s, fileData.begin() + s + 4);
    int32_t soundDataChunkSize = fourBytesToInt(fileData, s + 4, Endianness::BigEndian);
    int32_t offset             = fourBytesToInt(fileData, s + 8, Endianness::BigEndian);
    // int32_t blockSize = fourBytesToInt (fileData, s + 12, Endianness::BigEndian);

    int numBytesPerSample        = bitDepth_ / 8;
    int numBytesPerFrame         = numBytesPerSample * numChannels;
    int totalNumAudioSampleBytes = numSamplesPerChannel * numBytesPerFrame;
    int samplesStartIndex        = s + 16 + (int)offset;

    // sanity check the data
    if ((soundDataChunkSize - 8) != totalNumAudioSampleBytes
        || totalNumAudioSampleBytes > static_cast<long>(fileData.size() - samplesStartIndex))
    {
        reportError("ERROR: the metadatafor this file doesn't seem right");
        return false;
    }

    clearAudioBuffer();
    samples.resize(numChannels);

    for (int i = 0; i < numSamplesPerChannel; i++)
    {
        for (int channel = 0; channel < numChannels; channel++)
        {
            int sampleIndex = samplesStartIndex + (numBytesPerFrame * i) + channel * numBytesPerSample;

            if ((sampleIndex + (bitDepth_ / 8) - 1) >= fileData.size())
            {
                reportError(
                    "ERROR: read file error as the metadata indicates more samples than there are in the file data");
                return false;
            }

            if (bitDepth_ == 8)
            {
                auto sampleAsSigned8Bit = (int8_t)fileData[sampleIndex];
                T sample                = (T)sampleAsSigned8Bit / (T)128.;
                samples[channel].push_back(sample);
            }
            else if (bitDepth_ == 16)
            {
                int16_t sampleAsInt = twoBytesToInt(fileData, sampleIndex, Endianness::BigEndian);
                T sample            = sixteenBitIntToSample(sampleAsInt);
                samples[channel].push_back(sample);
            }
            else if (bitDepth_ == 24)
            {
                int32_t sampleAsInt = 0;
                sampleAsInt
                    = (fileData[sampleIndex] << 16) | (fileData[sampleIndex + 1] << 8) | fileData[sampleIndex + 2];

                if ((sampleAsInt & 0x800000) != 0)
                {  //  if the 24th bit is set, this is a negative number in 24-bit world
                    sampleAsInt = sampleAsInt | ~0xFFFFFF;  // so make sure sign is extended to the 32 bit float
                }

                T sample = (T)sampleAsInt / (T)8388608.;
                samples[channel].push_back(sample);
            }
            else if (bitDepth_ == 32)
            {
                int32_t sampleAsInt = fourBytesToInt(fileData, sampleIndex, Endianness::BigEndian);
                T sample;

                if (audioFormat == AIFFAudioFormat::Compressed) { sample = (T) reinterpret_cast<float&>(sampleAsInt); }
                else
                {  // assume uncompressed
                    sample = (T)sampleAsInt / static_cast<float>(std::numeric_limits<std::int32_t>::max());
                }

                samples[channel].push_back(sample);
            }
            else { MC_ASSERT(false); }
        }
    }

    // -----------------------------------------------------------
    // iXML CHUNK
    if (indexOfXMLChunk != -1)
    {
        int32_t chunkSize = fourBytesToInt(fileData, indexOfXMLChunk + 4);
        iXMLChunk         = std::string((char const*)&fileData[indexOfXMLChunk + 8], chunkSize);
    }

    return true;
}

template<class T>
auto AudioFile<T>::getAiffSampleRate(std::vector<uint8_t>& fileData, int sampleRateStartIndex) -> uint32_t
{
    for (auto it : aiffSampleRateTable)
    {
        if (tenByteMatch(fileData, sampleRateStartIndex, it.second, 0)) { return it.first; }
    }

    return 0;
}

template<class T>
auto AudioFile<T>::tenByteMatch(std::vector<uint8_t>& v1, int startIndex1, std::vector<uint8_t>& v2, int startIndex2)
    -> bool
{
    for (int i = 0; i < 10; i++)
    {
        if (v1[startIndex1 + i] != v2[startIndex2 + i]) { return false; }
    }

    return true;
}

template<class T>
void AudioFile<T>::addSampleRateToAiffData(std::vector<uint8_t>& fileData, uint32_t sampleRate)
{
    if (aiffSampleRateTable.count(sampleRate) > 0)
    {
        for (int i = 0; i < 10; i++) { fileData.push_back(aiffSampleRateTable[sampleRate][i]); }
    }
}

template<class T>
auto AudioFile<T>::save(std::string filePath, AudioFileFormat format) -> bool
{
    if (format == AudioFileFormat::Wave) { return saveToWaveFile(filePath); }
    if (format == AudioFileFormat::Aiff) { return saveToAiffFile(filePath); }

    return false;
}

template<class T>
auto AudioFile<T>::saveToWaveFile(std::string filePath) -> bool
{
    std::vector<uint8_t> fileData;

    int32_t dataChunkSize   = getNumSamplesPerChannel() * (getNumChannels() * bitDepth_ / 8);
    int16_t audioFormat     = bitDepth_ == 32 ? WavAudioFormat::IEEEFloat : WavAudioFormat::PCM;
    int32_t formatChunkSize = audioFormat == WavAudioFormat::PCM ? 16 : 18;
    auto iXMLChunkSize      = static_cast<int32_t>(iXMLChunk.size());

    // -----------------------------------------------------------
    // HEADER CHUNK
    addStringToFileData(fileData, "RIFF");

    // The file size in bytes is the header chunk size (4, not counting RIFF and WAVE) + the format
    // chunk size (24) + the metadata part of the data chunk plus the actual data chunk size
    int32_t fileSizeInBytes = 4 + formatChunkSize + 8 + 8 + dataChunkSize;
    if (iXMLChunkSize > 0) { fileSizeInBytes += (8 + iXMLChunkSize); }

    addInt32ToFileData(fileData, fileSizeInBytes);

    addStringToFileData(fileData, "WAVE");

    // -----------------------------------------------------------
    // FORMAT CHUNK
    addStringToFileData(fileData, "fmt ");
    addInt32ToFileData(fileData, formatChunkSize);            // format chunk size (16 for PCM)
    addInt16ToFileData(fileData, audioFormat);                // audio format
    addInt16ToFileData(fileData, (int16_t)getNumChannels());  // num channels
    addInt32ToFileData(fileData, (int32_t)sampleRate_);       // sample rate

    auto numBytesPerSecond = (int32_t)((getNumChannels() * sampleRate_ * bitDepth_) / 8);
    addInt32ToFileData(fileData, numBytesPerSecond);

    int16_t numBytesPerBlock = getNumChannels() * (bitDepth_ / 8);
    addInt16ToFileData(fileData, numBytesPerBlock);

    addInt16ToFileData(fileData, (int16_t)bitDepth_);

    if (audioFormat == WavAudioFormat::IEEEFloat)
    {
        addInt16ToFileData(fileData, 0);  // extension size
    }

    // -----------------------------------------------------------
    // DATA CHUNK
    addStringToFileData(fileData, "data");
    addInt32ToFileData(fileData, dataChunkSize);

    for (int i = 0; i < getNumSamplesPerChannel(); i++)
    {
        for (int channel = 0; channel < getNumChannels(); channel++)
        {
            if (bitDepth_ == 8)
            {
                uint8_t byte = sampleToSingleByte(samples[channel][i]);
                fileData.push_back(byte);
            }
            else if (bitDepth_ == 16)
            {
                int16_t sampleAsInt = sampleToSixteenBitInt(samples[channel][i]);
                addInt16ToFileData(fileData, sampleAsInt);
            }
            else if (bitDepth_ == 24)
            {
                auto sampleAsIntAgain = (int32_t)(samples[channel][i] * (T)8388608.);

                uint8_t bytes[3];
                bytes[2] = (uint8_t)(sampleAsIntAgain >> 16) & 0xFF;
                bytes[1] = (uint8_t)(sampleAsIntAgain >> 8) & 0xFF;
                bytes[0] = (uint8_t)sampleAsIntAgain & 0xFF;

                fileData.push_back(bytes[0]);
                fileData.push_back(bytes[1]);
                fileData.push_back(bytes[2]);
            }
            else if (bitDepth_ == 32)
            {
                int32_t sampleAsInt = 0;

                if (audioFormat == WavAudioFormat::IEEEFloat)
                {
                    sampleAsInt = (int32_t) reinterpret_cast<int32_t&>(samples[channel][i]);
                }
                else
                {  // assume PCM
                    sampleAsInt = (int32_t)(samples[channel][i] * std::numeric_limits<int32_t>::max());
                }

                addInt32ToFileData(fileData, sampleAsInt, Endianness::LittleEndian);
            }
            else
            {
                MC_ASSERT(false && "Trying to write a file with unsupported bit depth");
                return false;
            }
        }
    }

    // -----------------------------------------------------------
    // iXML CHUNK
    if (iXMLChunkSize > 0)
    {
        addStringToFileData(fileData, "iXML");
        addInt32ToFileData(fileData, iXMLChunkSize);
        addStringToFileData(fileData, iXMLChunk);
    }

    // check that the various sizes we put in the metadata are correct
    if (fileSizeInBytes != static_cast<int32_t>(fileData.size() - 8)
        || dataChunkSize != (getNumSamplesPerChannel() * getNumChannels() * (bitDepth_ / 8)))
    {
        reportError("ERROR: couldn't save file to " + filePath);
        return false;
    }

    // try to write the file
    return writeDataToFile(fileData, filePath);
}

template<class T>
auto AudioFile<T>::saveToAiffFile(std::string filePath) -> bool
{
    std::vector<uint8_t> fileData;

    int32_t numBytesPerSample        = bitDepth_ / 8;
    int32_t numBytesPerFrame         = numBytesPerSample * getNumChannels();
    int32_t totalNumAudioSampleBytes = getNumSamplesPerChannel() * numBytesPerFrame;
    int32_t soundDataChunkSize       = totalNumAudioSampleBytes + 8;
    auto iXMLChunkSize               = static_cast<int32_t>(iXMLChunk.size());

    // -----------------------------------------------------------
    // HEADER CHUNK
    addStringToFileData(fileData, "FORM");

    // The file size in bytes is the header chunk size (4, not counting FORM and AIFF) + the COMM
    // chunk size (26) + the metadata part of the SSND chunk plus the actual data chunk size
    int32_t fileSizeInBytes = 4 + 26 + 16 + totalNumAudioSampleBytes;
    if (iXMLChunkSize > 0) { fileSizeInBytes += (8 + iXMLChunkSize); }

    addInt32ToFileData(fileData, fileSizeInBytes, Endianness::BigEndian);

    addStringToFileData(fileData, "AIFF");

    // -----------------------------------------------------------
    // COMM CHUNK
    addStringToFileData(fileData, "COMM");
    addInt32ToFileData(fileData, 18, Endianness::BigEndian);                         // commChunkSize
    addInt16ToFileData(fileData, getNumChannels(), Endianness::BigEndian);           // num channels
    addInt32ToFileData(fileData, getNumSamplesPerChannel(), Endianness::BigEndian);  // num samples per channel
    addInt16ToFileData(fileData, bitDepth_, Endianness::BigEndian);                  // bit depth
    addSampleRateToAiffData(fileData, sampleRate_);

    // -----------------------------------------------------------
    // SSND CHUNK
    addStringToFileData(fileData, "SSND");
    addInt32ToFileData(fileData, soundDataChunkSize, Endianness::BigEndian);
    addInt32ToFileData(fileData, 0, Endianness::BigEndian);  // offset
    addInt32ToFileData(fileData, 0, Endianness::BigEndian);  // block size

    for (int i = 0; i < getNumSamplesPerChannel(); i++)
    {
        for (int channel = 0; channel < getNumChannels(); channel++)
        {
            if (bitDepth_ == 8)
            {
                uint8_t byte = sampleToSingleByte(samples[channel][i]);
                fileData.push_back(byte);
            }
            else if (bitDepth_ == 16)
            {
                int16_t sampleAsInt = sampleToSixteenBitInt(samples[channel][i]);
                addInt16ToFileData(fileData, sampleAsInt, Endianness::BigEndian);
            }
            else if (bitDepth_ == 24)
            {
                auto sampleAsIntAgain = (int32_t)(samples[channel][i] * (T)8388608.);

                uint8_t bytes[3];
                bytes[0] = (uint8_t)(sampleAsIntAgain >> 16) & 0xFF;
                bytes[1] = (uint8_t)(sampleAsIntAgain >> 8) & 0xFF;
                bytes[2] = (uint8_t)sampleAsIntAgain & 0xFF;

                fileData.push_back(bytes[0]);
                fileData.push_back(bytes[1]);
                fileData.push_back(bytes[2]);
            }
            else if (bitDepth_ == 32)
            {
                // write samples as signed integers (no implementation yet for floating point, but looking at WAV
                // implementation should help)
                auto sampleAsInt = (int32_t)(samples[channel][i] * std::numeric_limits<int32_t>::max());
                addInt32ToFileData(fileData, sampleAsInt, Endianness::BigEndian);
            }
            else
            {
                MC_ASSERT(false && "Trying to write a file with unsupported bit depth");
                return false;
            }
        }
    }

    // -----------------------------------------------------------
    // iXML CHUNK
    if (iXMLChunkSize > 0)
    {
        addStringToFileData(fileData, "iXML");
        addInt32ToFileData(fileData, iXMLChunkSize, Endianness::BigEndian);
        addStringToFileData(fileData, iXMLChunk);
    }

    // check that the various sizes we put in the metadata are correct
    if (fileSizeInBytes != static_cast<int32_t>(fileData.size() - 8)
        || soundDataChunkSize != getNumSamplesPerChannel() * numBytesPerFrame + 8)
    {
        reportError("ERROR: couldn't save file to " + filePath);
        return false;
    }

    // try to write the file
    return writeDataToFile(fileData, filePath);
}

template<class T>
auto AudioFile<T>::writeDataToFile(std::vector<uint8_t>& fileData, std::string const& filePath) -> bool
{
    std::ofstream outputFile(filePath, std::ios::binary);

    if (outputFile.is_open())
    {
        for (unsigned char i : fileData)
        {
            char value = (char)i;
            outputFile.write(&value, sizeof(char));
        }

        outputFile.close();

        return true;
    }

    return false;
}

template<class T>
void AudioFile<T>::addStringToFileData(std::vector<uint8_t>& fileData, std::string const& s)
{
    for (char i : s) { fileData.push_back((uint8_t)i); }
}

template<class T>
void AudioFile<T>::addInt32ToFileData(std::vector<uint8_t>& fileData, int32_t i, Endianness endianness)
{
    uint8_t bytes[4];

    if (endianness == Endianness::LittleEndian)
    {
        bytes[3] = (i >> 24) & 0xFF;
        bytes[2] = (i >> 16) & 0xFF;
        bytes[1] = (i >> 8) & 0xFF;
        bytes[0] = i & 0xFF;
    }
    else
    {
        bytes[0] = (i >> 24) & 0xFF;
        bytes[1] = (i >> 16) & 0xFF;
        bytes[2] = (i >> 8) & 0xFF;
        bytes[3] = i & 0xFF;
    }

    for (unsigned char& byte : bytes) { fileData.push_back(byte); }
}

template<class T>
void AudioFile<T>::addInt16ToFileData(std::vector<uint8_t>& fileData, int16_t i, Endianness endianness)
{
    uint8_t bytes[2];

    if (endianness == Endianness::LittleEndian)
    {
        bytes[1] = (i >> 8) & 0xFF;
        bytes[0] = i & 0xFF;
    }
    else
    {
        bytes[0] = (i >> 8) & 0xFF;
        bytes[1] = i & 0xFF;
    }

    fileData.push_back(bytes[0]);
    fileData.push_back(bytes[1]);
}

template<class T>
void AudioFile<T>::clearAudioBuffer()
{
    for (size_t i = 0; i < samples.size(); i++) { samples[i].clear(); }

    samples.clear();
}

template<class T>
auto AudioFile<T>::determineAudioFileFormat(std::vector<uint8_t>& fileData) -> AudioFileFormat
{
    std::string header(fileData.begin(), fileData.begin() + 4);

    if (header == "RIFF") { return AudioFileFormat::Wave; }
    if (header == "FORM") { return AudioFileFormat::Aiff; }
    return AudioFileFormat::Error;
}

template<class T>
auto AudioFile<T>::fourBytesToInt(std::vector<uint8_t>& source, int startIndex, Endianness endianness) -> int32_t
{
    int32_t result = 0;

    if (endianness == Endianness::LittleEndian)
    {
        result = (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) | (source[startIndex + 1] << 8)
                 | source[startIndex];
    }
    else
    {
        result = (source[startIndex] << 24) | (source[startIndex + 1] << 16) | (source[startIndex + 2] << 8)
                 | source[startIndex + 3];
    }

    return result;
}

template<class T>
auto AudioFile<T>::twoBytesToInt(std::vector<uint8_t>& source, int startIndex, Endianness endianness) -> int16_t
{
    int16_t result = 0;

    if (endianness == Endianness::LittleEndian) { result = (source[startIndex + 1] << 8) | source[startIndex]; }
    else { result = (source[startIndex] << 8) | source[startIndex + 1]; }

    return result;
}

template<class T>
auto AudioFile<T>::getIndexOfString(std::vector<uint8_t>& source, std::string const& stringToSearchFor) -> int
{
    int index        = -1;
    int stringLength = (int)stringToSearchFor.length();

    for (size_t i = 0; i < source.size() - stringLength; i++)
    {
        std::string section(source.begin() + i, source.begin() + i + stringLength);

        if (section == stringToSearchFor)
        {
            index = static_cast<int>(i);
            break;
        }
    }

    return index;
}

template<class T>
auto AudioFile<T>::getIndexOfChunk(std::vector<uint8_t>& source, std::string const& chunkHeaderID, int startIndex,
                                   Endianness endianness) -> int
{
    constexpr int dataLen = 4;
    if (chunkHeaderID.size() != dataLen)
    {
        MC_ASSERT(false && "Invalid chunk header ID string");
        return -1;
    }

    int i = startIndex;
    while (i < source.size() - dataLen)
    {
        if (memcmp(&source[i], chunkHeaderID.data(), dataLen) == 0) { return i; }

        i += dataLen;
        auto chunkSize = fourBytesToInt(source, i, endianness);
        i += (dataLen + chunkSize);
    }

    return -1;
}

template<class T>
auto AudioFile<T>::sixteenBitIntToSample(int16_t sample) -> T
{
    return static_cast<T>(sample) / static_cast<T>(32768.);
}

template<class T>
auto AudioFile<T>::sampleToSixteenBitInt(T sample) -> int16_t
{
    sample = clamp(sample, -1., 1.);
    return static_cast<int16_t>(sample * 32767.);
}

template<class T>
auto AudioFile<T>::sampleToSingleByte(T sample) -> uint8_t
{
    sample = clamp(sample, -1., 1.);
    sample = (sample + 1.) / 2.;
    return static_cast<uint8_t>(sample * 255.);
}

template<class T>
auto AudioFile<T>::singleByteToSample(uint8_t sample) -> T
{
    return static_cast<T>(sample - 128) / static_cast<T>(128.);
}

template<class T>
auto AudioFile<T>::clamp(T value, T minValue, T maxValue) -> T
{
    value = std::min(value, maxValue);
    value = std::max(value, minValue);
    return value;
}

template<class T>
void AudioFile<T>::reportError(std::string const& errorMessage)
{
    if (logErrorsToConsole_) { fmt::print("{0}\n", errorMessage); }
}

#if defined(_MSC_VER)
__pragma(warning(pop))
#elif defined(__GNUC__)
_Pragma("GCC diagnostic pop")
#endif

#endif /* AudioFile_h */
