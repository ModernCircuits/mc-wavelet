#pragma once

#include "lt/format.hpp"

#include <cstdlib>
#include <iterator>
#include <vector>

inline auto readFileToVector(char const* filePath) -> std::vector<double>
{
    auto* ifp = std::fopen(filePath, "r");
    if (ifp == nullptr) {
        fmt::printf("Cannot Open File: %s\n", filePath);
        std::exit(EXIT_FAILURE);
    }

    auto result = std::vector<double> {};
    result.reserve(8096 * 4);
    while (std::feof(ifp) == 0) {
        auto temp = 0.0;
        std::fscanf(ifp, "%lf\n", &temp);
        result.push_back(temp);
    }

    std::fclose(ifp);
    return result;
}