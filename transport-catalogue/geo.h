/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <cmath>
#include <string>
#include <vector>

#include "common.h"
#include "svgLib/svglib.h"

namespace transport_catalogue::geo {
    class Geo {
        public:
            static double ComputeDistance(const common::Coordinates& arg_from , const common::Coordinates& arg_to) {
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

            static svglib::sPoint GeoCoordToDrawCoord(common::Coordinates            arg_coordinates
                                                    , const common::sMapData&        arg_map_data
                                                    , const common::sRenderSettings& arg_render_settings);
            static common::sMapData CalculateMapData(const std::vector<const common::sStop*>& arg_stops
                                                   , const common::sRenderSettings&           arg_render_settings);
        private:
            static constexpr int    kEarth_radius   = 6371000;
            static constexpr double kDegrees_per_pi = 180.;
            static constexpr double kPi             = 3.1415926535;
    };
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
