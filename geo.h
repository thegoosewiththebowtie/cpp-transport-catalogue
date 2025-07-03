/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <cmath>

namespace transport_catalogue::geo {
    class Geo {
        public:
            struct Coordinates {
                double lat;
                double lng;

                bool operator==(const Coordinates& arg_other) const {
                    return lat == arg_other.lat && lng == arg_other.lng;
                }

                bool operator!=(const Coordinates& arg_other) const { return !(*this == arg_other); }
            };

            static double ComputeDistance(const Coordinates& arg_from , const Coordinates& arg_to) {
                {
                    if(arg_from == arg_to) { return 0.; }
                    static constexpr double kConversion_factor = kPi / kDegrees_per_pi;
                    return std::acos(std::sin(arg_from.lat * kConversion_factor) *
                                     std::sin(arg_to.lat * kConversion_factor)
                                     + std::cos(arg_from.lat * kConversion_factor) *
                                     std::cos(arg_to.lat * kConversion_factor) *
                                     std::cos(std::abs(arg_from.lng - arg_to.lng) * kConversion_factor)) * static_cast<
                        double>(kEarth_radius);
                }
            }

            static double GetRouteCurvature(const double arg_f_distance , const double arg_g_distance) {
                return arg_f_distance / arg_g_distance;
            }
        private:
            static constexpr int    kEarth_radius   = 6371000;
            static constexpr auto   kDegrees_per_pi = static_cast<double>(180);
            static constexpr double kPi             = 3.1415926535;
    };
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
