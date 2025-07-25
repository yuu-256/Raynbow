#include "scene.hpp"
#include <cmath>
#include <random>
#include <iostream>

Scene::Scene(double box_size, double mean_free_path, 
             std::function<std::array<double, 2>(double)> direction_sampler)
    : L(box_size), mfp(mean_free_path), sample_direction(direction_sampler),
    rng(std::random_device{}()), uniform01(0.0, 1) {}

void Scene::trace_photon(Photon& photon) {
    auto [theta, phi] = sample_direction(photon.wavelength);
    record_scattering_angle({theta, phi});
    photon.direction = scatter_direction_from_angles(theta, phi, photon.direction);

    double dz = photon.direction[2];
    if (dz <= 0) {
        photon.alive = false; // Photon is moving away from the screen
        return;
    }

    double t = (L - photon.position[2]) / dz;
    photon.position[0] += t * photon.direction[0];
    photon.position[1] += t * photon.direction[1];
    photon.position[2] = L; // Set z to the screen level

    screen_hits.push_back(photon.position);
    photon.alive = false; // Photon is absorbed by the screen
}

// void Scene::trace_photon(Photon& photon) {
//     while (photon.alive) {
//         std::array<double, 3> prev_position = photon.position;
// 
//         // Move to next scattering point
//         // double step = -mfp * std::log(uniform01(rng));
//         double step = -mfp * std::log(0.5);
//         photon.position[0] += step * photon.direction[0];
//         photon.position[1] += step * photon.direction[1];
//         photon.position[2] += step * photon.direction[2];
// 
//         if (prev_position[2] < L && photon.position[2] >= L) {
//             double dz = photon.position[2] - prev_position[2];
//             double t = (L - prev_position[2]) / dz;
//         
// 
//             std::array<double, 3> intersection = {
//                 prev_position[0] + t * (photon.position[0] - prev_position[0]),
//                 prev_position[1] + t * (photon.position[1] - prev_position[1]),
//                 L
//             };
// 
//             screen_hits.push_back(intersection);
//             photon.alive = false; // Photon is absorbed by the screen
//             return;
//         }
//  
//         if (std::abs(photon.position[0]) > L * 10 || 
//             std::abs(photon.position[1]) > L * 10) {
//             // Photon is lost (absorbed by the walls)
//             photon.alive = false;
//             return;
//         }
//  
//         // Scatter
//         auto [theta, phi] = sample_direction(photon.wavelength);
//         record_scattering_angle({theta, phi});
//         photon.direction = scatter_direction_from_angles(theta, phi, photon.direction);
//         // photon.direction = sample_direction(photon.wavelength);
//     }
// }

void Scene::record_scattering_angle(const std::array<double, 2>& angles) {
    scattering_angles.push_back(angles[0]);
}

// void Scene::record_scattering_angle(const std::array<double, 3>& direction) {
//     double cos_theta = -direction[2];
//     cos_theta = std::clamp(cos_theta, -1.0, 1.0);
//     scattering_angles.push_back(std::acos(cos_theta));
// }

std::array<double, 3> Scene::scatter_direction_from_angles(
    double theta, double phi,
    const std::array<double, 3>& current_dir) {
    const auto& z = current_dir;
    std::array<double, 3> a = (std::abs(z[0])<0.9) ? std::array<double, 3>{1, 0, 0} : std::array<double, 3>{0, 1, 0};

    std::array<double, 3> y = {
        z[1]*a[2] - z[2]*a[1],
        z[2]*a[0] - z[0]*a[2],
        z[0]*a[1] - z[1]*a[0]
    };
    double norm = std::sqrt(y[0]*y[0] + y[1]*y[1] + y[2]*y[2]);
    for (int i = 0; i < 3; ++i) {
        y[i] /= norm;
    }

    std::array<double, 3> x = {
        y[1]*z[2] - y[2]*z[1],
        y[2]*z[0] - y[0]*z[2],
        y[0]*z[1] - y[1]*z[0]
    };

    double sin_theta = std::sin(theta);
    std::array<double, 3> local = {
        sin_theta * std::cos(phi),
        sin_theta * std::sin(phi),
        std::cos(theta)
    };

    return {
        local[0] * x[0] + local[1] * y[0] + local[2] * z[0],
        local[0] * x[1] + local[1] * y[1] + local[2] * z[1],
        local[0] * x[2] + local[1] * y[2] + local[2] * z[2]
    };
}

const std::vector<double>& Scene::get_scattering_angles() const {
    return scattering_angles;
}

const std::vector<std::array<double, 3>>& Scene::get_hits() const {
    return screen_hits;
}
