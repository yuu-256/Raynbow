#pragma once
#include "photon.hpp"
#include <vector>
#include <functional>

class Scene {
public:
    Scene(double box_size, double mean_free_path, 
        std::function<std::array<double, 2>(double)> direction_sampler);
    void trace_photon(Photon& photon);
    const std::vector<std::array<double, 3>>& get_hits() const;
    const std::vector<double>& get_scattering_angles() const;

private:
    double L;
    double mfp;
    std::function<std::array<double, 2>(double)> sample_direction;
    std::vector<std::array<double, 3>> screen_hits;
    std::mt19937 rng;
    std::uniform_real_distribution<double> uniform01;
    std::vector<double> scattering_angles;
    std::array<double, 3> scatter_direction_from_angles(double theta, double phi, const std::array<double, 3>& prev_direction);
    void record_scattering_angle(const std::array<double, 2>& angles);
};
