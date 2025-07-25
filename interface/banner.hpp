#pragma once
#include <iostream>
#include <cmath>

void print_banner() {
    const char* lines[] = {
        R"(88888888ba                                         88                                            )",
        R"(88      "8b                                        88                                            )",
        R"(88      ,8P                                        88                                            )",
        R"(88aaaaaa8P'  ,adPPYYba,  8b       d8  8b,dPPYba,   88,dPPYba,    ,adPPYba,   8b      db      d8  )",
        R"(88""""88'    ""     `Y8  `8b     d8'  88P'   `"8a  88P'    "8a  a8"     "8a  `8b    d88b    d8'  )",
        R"(88    `8b    ,adPPPPP88   `8b   d8'   88       88  88       d8  8b       d8   `8b  d8'`8b  d8'   )",
        R"(88     `8b   88,    ,88    `8b,d8'    88       88  88b,   ,a8"  "8a,   ,a8"    `8bd8'  `8bd8'    )",
        R"(88      `8b  `"8bbdP"Y8      Y88'     88       88  8Y"Ybbd8"'    `"YbbdP"'       YP      YP     )",
        R"(                             d8'                                                               )",
        R"(                            d8'                                                                )"
    };
    const int n_lines = sizeof(lines) / sizeof(lines[0]);

    std::cout << "\n";

    for (int i = 0; i < n_lines; ++i) {
        double t = static_cast<double>(i) / n_lines * 2.0 * M_PI + M_PI / 4.0;
        int r = static_cast<int>(std::sin(0.8 * t) * 127 + 128);
        int g = static_cast<int>(std::sin(0.8 * t - 2 * M_PI / 3.0) * 127 + 128);
        int b = static_cast<int>(std::sin(0.8 * t - 4 * M_PI / 3.0) * 127 + 128);

        std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m"
                  << lines[i] << "\033[0m" << std::endl;
    }

    std::cout << "\n";
    std::cout << "\033[1mRaynbow: LX-MIE and C++ Ray Tracing Rainbow Simulator - Version 0.0\033[0m" << std::endl;
}
