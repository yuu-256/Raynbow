#pragma once
#include <array>
#include <random>

struct Photon {
    std::array<double, 3> position;  // x, y, z coordinates
    std::array<double, 3> direction; // direction vector (dx, dy, dz)
    double wavelength;               // wavelength in nanometers
    bool alive = true;                      // whether the photon is still active

    Photon(std::array<double, 3> pos, std::array<double, 3> dir, double lambda)
        : position(pos), direction(dir), wavelength(lambda) {}
};

inline std::array<double, 3> normalize(const std::array<double, 3>& vec) {
    double norm = std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
    if (norm == 0) return {0, 0, 0}; // Avoid division by zero
    return {vec[0] / norm, vec[1] / norm, vec[2] / norm};
}
