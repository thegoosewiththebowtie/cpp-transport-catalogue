/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */

#include "geo.h"


namespace transport_catalogue::geo {
    common::sMapData Geo::CalculateMapData(const std::vector<const common::sStop*>& arg_stops
                                         , const common::sRenderSettings&           arg_render_settings) {
        double min_lat = arg_stops.front()->coordinates.lat;
        double max_lat{min_lat};
        double min_lng = arg_stops.front()->coordinates.lng;
        double max_lng{min_lng};
        for(const common::sStop* const stop : arg_stops) {
            min_lat = std::min(min_lat, stop->coordinates.lat);
            min_lng = std::min(min_lng, stop->coordinates.lng);
            max_lat = std::max(max_lat, stop->coordinates.lat);
            max_lng = std::max(max_lng, stop->coordinates.lng);
        }
        constexpr double epsilon  = 1e-6;
        const double     lng_diff = max_lng - min_lng;
        const double     lat_diff = max_lat - min_lat;
        const double     xzmod    = (std::abs(lng_diff) < epsilon)
                                        ? 0.0
                                        : (arg_render_settings.width - 2 * arg_render_settings.padding) / lng_diff;
        const double yzmod = (std::abs(lat_diff) < epsilon)
                                 ? 0.0
                                 : (arg_render_settings.height - 2 * arg_render_settings.padding) / lat_diff;
        double czmod;
        if(static_cast<bool>(xzmod) && static_cast<bool>(yzmod)) { czmod = std::min(xzmod, yzmod); }
        else { czmod = std::max(xzmod, yzmod); }
        return {min_lat , min_lng , max_lat , max_lng , xzmod , yzmod , czmod};
    }

    svglib::sPoint Geo::GeoCoordToDrawCoord(const common::Coordinates              arg_coordinates
                                          , const common::sMapData&        arg_map_data
                                          , const common::sRenderSettings& arg_render_settings) {
        return {(arg_coordinates.lng - arg_map_data.min_lng) * arg_map_data.czmod + arg_render_settings.padding
              , (arg_map_data.max_lat - arg_coordinates.lat) * arg_map_data.czmod + arg_render_settings.padding};
    }
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
