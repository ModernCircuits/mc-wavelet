#pragma once

#include "mc/preprocessor.hpp"

#if __has_include(<bit>)
    #include <bit>
#endif

#if defined(__cpp_lib_bit_cast)
namespace mc
{
using std::bit_cast;
}
#else
namespace mc
{

}  // namespace mc
#endif