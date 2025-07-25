#include "phase_function_reader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>

PhaseFunctionSampler::PhaseFunctionSampler(const std::string& filepath, const std::vector<double>& wavelength_list)
    : rng(std::random_device{}()), uniform01(0.0, 1.0) {
    std::ifstream fin(filepath);
    if (!fin) {
        throw std::runtime_error("Failed to open phase function file: " + filepath);
    }

    std::string line;
    std::getline(fin, line); // skip header

    std::vector<std::vector<double>> raw_data;
    while (std::getline(fin, line)) {
        std::istringstream ss(line);
        std::vector<double> row;
        double val;
        while (ss >> val) row.push_back(val);
        raw_data.push_back(std::move(row));
    }

    size_t nwav = raw_data[0].size() - 1;
    if (wavelength_list.size() != nwav) {
        throw std::runtime_error("Mismatch between wavelength list and data columns");
    }

    for (size_t w = 0; w < nwav; ++w) {
        PhaseFunctionTable table;
        table.cos_theta.reserve(raw_data.size());
        table.P_theta.reserve(raw_data.size());
        table.cdf.reserve(raw_data.size());

        for (const auto& row : raw_data) {
            double cos_t = row[0];
            // double theta = std::acos(cos_t);
            // double weight = std::sin(theta);
            table.cos_theta.push_back(cos_t);
            // table.P_theta.push_back(row[w+1] * weight);
            table.P_theta.push_back(row[w+1]);
        }

        // Build normalized CDF
        table.cdf.push_back(0.0);
        for (size_t i = 1; i < table.P_theta.size(); ++i) {
            double dmu = std::abs(table.cos_theta[i] - table.cos_theta[i-1]);
            double area = 0.5 * dmu * (table.P_theta[i] + table.P_theta[i-1]);
            table.cdf.push_back(table.cdf.back() + area);
        }
        for (double& v : table.cdf) v /= table.cdf.back();

        double lambda = wavelength_list[w];
        tables[lambda] = std::move(table);
    }
}

std::array<double, 2> PhaseFunctionSampler::sample_direction(double wavelength) {
    auto it = tables.lower_bound(wavelength);
    if (it == tables.end()) --it;
    auto& table = it->second;

    double xi = uniform01(rng);
    auto it_cdf = std::lower_bound(table.cdf.begin(), table.cdf.end(), xi);
    size_t idx = std::clamp<size_t>(std::distance(table.cdf.begin(), it_cdf), 1, table.cdf.size() - 1);

    double cdf0 = table.cdf[idx - 1];
    double cdf1 = table.cdf[idx];
    double mu0 = table.cos_theta[idx - 1];
    double mu1 = table.cos_theta[idx];

    double t = (xi - cdf0) / (cdf1 - cdf0 + 1e-12);  // avoid division by zero
    double mu = mu0 + t * (mu1 - mu0);
    double theta = std::acos(std::clamp(mu, -1.0, 1.0));
    double phi = 2.0 * M_PI * uniform01(rng);

    return {theta, phi};
}

// std::array<double, 3> PhaseFunctionSampler::sample_direction(double wavelength) {
//     auto it = tables.lower_bound(wavelength);
//     if (it == tables.end()) --it;
//     auto& table = it->second;
// 
//     double xi = uniform01(rng);
// 
//     auto it_cdf = std::lower_bound(table.cdf.begin(), table.cdf.end(), xi);
//     size_t idx = std::clamp<size_t>(std::distance(table.cdf.begin(), it_cdf), 1, table.cdf.size() - 1);
// 
//     double cdf0 = table.cdf[idx - 1];
//     double cdf1 = table.cdf[idx];
//     double mu0 = table.cos_theta[idx - 1];
//     double mu1 = table.cos_theta[idx];
// 
//     double t = (xi - cdf0) / (cdf1 - cdf0 + 1e-12);
//     double mu = mu0 + t * (mu1 - mu0);
//     double theta = std::acos(std::clamp(mu, -1.0, 1.0));
// 
//     double phi = 2.0 * M_PI * uniform01(rng);
// 
//     return {
//         std::sin(theta) * std::cos(phi),
//         std::sin(theta) * std::sin(phi),
//         std::cos(theta)
//     };
// }

// std::array<double, 3> PhaseFunctionSampler::sample_direction(double wavelength) {
//     auto it = tables.lower_bound(wavelength);
//     if (it == tables.end()) --it;
//     auto& table = it->second;
// 
//     double xi = uniform01(rng);
// 
//     auto it_cdf = std::lower_bound(table.cdf.begin(), table.cdf.end(), xi);
//     size_t idx = std::clamp<size_t>(std::distance(table.cdf.begin(), it_cdf), 1, table.cdf.size() - 1);
// 
//     double mu = table.cos_theta[idx];
//     double theta = std::acos(mu);
//     double phi = 2.0 * M_PI * uniform01(rng);
// 
//     return {
//         std::sin(theta) * std::cos(phi),
//         std::sin(theta) * std::sin(phi),
//         std::cos(theta)
//     };
// }
