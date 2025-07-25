#pragma once
#include <map>
#include <vector>
#include <string>
#include <array>
#include <random>

class PhaseFunctionSampler {
public:
    PhaseFunctionSampler(const std::string& filepath, const std::vector<double>& wavelength_list);
    std::array<double, 2> sample_direction(double wavelength);

private:
    struct PhaseFunctionTable {
        std::vector<double> cos_theta;
        std::vector<double> P_theta;
        std::vector<double> cdf;
    };

    std::map<double, PhaseFunctionTable> tables;
    std::mt19937 rng;
    std::uniform_real_distribution<double> uniform01;
};
