#pragma once

#include <cmath>

class Geo {
public:
    struct Coordinates {
        double lat;
        double lng;

        bool operator==(const Coordinates &other) const {
            return lat == other.lat && lng == other.lng;
        }

        bool operator!=(const Coordinates &other) const {
            return !(*this == other);
        }
    };

    static double ComputeDistance(const Coordinates& from, const Coordinates& to) {
        constexpr int EARTH_RADIUS = 6371000;
        constexpr double DEGREES_PER_PI = 180;
        constexpr double PI = 3.1415926535;
        if (from == to) { return 0; }
        static constexpr double CONVERSION_FACTOR =  PI/DEGREES_PER_PI;
        return std::acos(std::sin(from.lat * CONVERSION_FACTOR)
                        * std::sin(to.lat * CONVERSION_FACTOR)
                    + std::cos(from.lat * CONVERSION_FACTOR)
                        * std::cos(to.lat * CONVERSION_FACTOR)
                        * std::cos(std::abs(from.lng - to.lng) * CONVERSION_FACTOR)
                    ) * EARTH_RADIUS;
    }

    static double GetRouteCurvature(const double f_distance, const double g_distance) {
        return f_distance / g_distance;
    }
};
