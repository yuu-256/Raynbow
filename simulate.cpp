#include "tracer/photon.hpp"
#include "tracer/scene.hpp"
#include "reader/phase_function_reader.hpp"
#include "reader/wavelengths_model.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <vector>
#include <map>


int main(int argc, char* argv[]) {

    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <phase_function_file> <lambda_min> <lambda_max>\n";
        return 1;
    }
    std::string phase_function_file = argv[1];
    const double lambda_min = std::atof(argv[2]) * 1e-3;
    const double lambda_max = std::atof(argv[3]) * 1e-3;

    const int N_photons_per_wavelength = 1000000;
    const double L = 1;               // Domain size
    const double mfp = 5;             // Mean free path
    const double lambda_step = 0.01;  // Wavelength step (10 um)

    // Create wavelength list
    std::vector<double> wavelength_list;
    for (double lambda = lambda_min; lambda <= lambda_max + 1e-6; lambda += lambda_step)
        wavelength_list.push_back(lambda);

    // Initialize phase function sampler
    std::vector<double> wavelengths_model = get_wavelengths_model();
    PhaseFunctionSampler sampler(phase_function_file, wavelengths_model);
    
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist_xy(-L/500, L/500);

    // Simulate for each wavelength
    for (double lambda : wavelength_list) {
        Scene scene(L, mfp, [&](double l) {
            return sampler.sample_direction(lambda);
        });

        for (int i = 0; i < N_photons_per_wavelength; ++i) {
            std::array<double, 3> origin = {dist_xy(rng), dist_xy(rng), 0};
            // std::array<double, 3> dir = {0, 0, -1}; // Circle
            std::array<double, 3> dir = {0.6, 0, -0.8}; // Oblique circle
            Photon photon(origin, dir, lambda);

            scene.trace_photon(photon);
            // multiple scattering
            // while (photon.alive) {
            //     scene.trace_photon(photon);
            // }
        }

        // Output results
        std::string filename = "output/output_lambda_" + std::to_string(lambda) + ".dat";
        std::ofstream ofs(filename);
        for (const auto& hit : scene.get_hits()) {
            ofs << hit[0] << "," << hit[1] << "," << hit[2] << "\n";
        }
        ofs.close();

        const auto& angles = scene.get_scattering_angles();
        double bin_width = 0.1;
        int n_bins = static_cast<int>(180 / bin_width);
        std::vector<int> histogram(n_bins, 0);

        for (double theta_rad : angles) {
            double theta_deg = theta_rad * 180.0 / M_PI;
            int bin = static_cast<int>(theta_deg / bin_width);
            if (bin >= 0 && bin < n_bins) histogram[bin]++;
        }
        std::ostringstream hist_filename;
        hist_filename << "output/histogram_lambda_" << lambda << ".csv";
        std::ofstream histfile(hist_filename.str());
        for (int i = 0; i < n_bins; ++i) {
            double angle = i * bin_width;
            histfile << angle << "," << histogram[i] << "\n";
        }
        histfile.close();

        // std::cout << "Lambda " << lambda << " completed. Output: " << filename << "\n";
    }

    // std::cout << "All wavelength simulations complete.\n";
    return 0;
}
