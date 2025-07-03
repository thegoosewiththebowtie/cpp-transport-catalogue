/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */

#include "maprenderer.h"

#include <algorithm>
#include <ranges>
#include <variant>

#include "common.h"
#include "transport_catalogue.h"
#include "jsonLib/jsonlib.h"
#include "salatLib/salatlib.h"

namespace transport_catalogue::map_renderer {
    void MapRenderer::Render(std::ostream& arg_output) {
        SALAT_SALTBEGIN();
        svglib::Document              todraw;
        std::vector<svglib::Polyline> routes;
        std::vector<svglib::Text>     routenames;
        RenderRoutes(routes, routenames);
        std::vector<svglib::Circle> stopcircles;
        std::vector<svglib::Text>   stopnames;
        RenderStops(stopnames, stopcircles);
        for(svglib::Polyline& route : routes) { todraw.Add(std::move(route)); }
        for(svglib::Text& routename : routenames) { todraw.Add(std::move(routename)); }
        for(svglib::Circle& stopscircle : stopcircles) { todraw.Add(std::move(stopscircle)); }
        for(svglib::Text& stopname : stopnames) { todraw.Add(std::move(stopname)); }
        todraw.Render(arg_output);
        SALAT_SALTEND();
    }

    sRenderSettings MapRenderer::SetupRenderSettings(const std::map<std::string , jsonlib::Node>& arg_render_settings) {
        SALAT_SALTBEGIN();
        sRenderSettings render_settings{
            arg_render_settings.find("width") == arg_render_settings.end()
                ? SALAT_RINWAR(0.0)
                : arg_render_settings.at("width").As<double>()
          , arg_render_settings.find("height") == arg_render_settings.end()
                ? SALAT_RINWAR(0.0)
                : arg_render_settings.at("height").As<double>()
          , arg_render_settings.find("padding") == arg_render_settings.end()
                ? SALAT_RINWAR(0.0)
                : arg_render_settings.at("padding").As<double>()
          , arg_render_settings.find("line_width") == arg_render_settings.end()
                ? SALAT_RINWAR(0.0)
                : arg_render_settings.at("line_width").As<double>()
          , arg_render_settings.find("stop_radius") == arg_render_settings.end()
                ? SALAT_RINWAR(0.0)
                : arg_render_settings.at("stop_radius").As<double>()
          , arg_render_settings.find("bus_label_font_size") == arg_render_settings.end()
                ? SALAT_RINWAR(0)
                : static_cast<unsigned>(arg_render_settings.at("bus_label_font_size").As<int>())
          , NodeToPoint(arg_render_settings.find("bus_label_offset") == arg_render_settings.end()
                            ? SALAT_RINWAR(0.0)
                            : arg_render_settings.at("bus_label_offset"))
          , arg_render_settings.find("stop_label_font_size") == arg_render_settings.end()
                ? SALAT_RINWAR(0)
                : static_cast<unsigned>(arg_render_settings.at("stop_label_font_size").As<int>())
          , NodeToPoint(arg_render_settings.find("stop_label_offset") == arg_render_settings.end()
                            ? SALAT_RINWAR(0.0)
                            : arg_render_settings.at("stop_label_offset"))
          , NodeToColor(arg_render_settings.find("underlayer_color") == arg_render_settings.end()
                            ? ""
                            : arg_render_settings.at("underlayer_color"))
          , arg_render_settings.find("underlayer_width") == arg_render_settings.end()
                ? SALAT_RINWAR(0.0)
                : arg_render_settings.at("underlayer_width").As<double>()};
        SALAT_CTMFBEGIN("AddColor");
        if(arg_render_settings.find("color_palette") != arg_render_settings.end()) {
            for(const jsonlib::Node& node : arg_render_settings.at("color_palette").As<jsonlib::array_TD>()) {
                render_settings.AddColor(NodeToColor(node));
                SALAT_CTMFBREAK("AddColor");
            }
        }
        else { SALAT_RINWAR(std::string("NOCOLOR")); }
        SALAT_CTMFEND("AddColor");
        SALAT_SALTEND();
        return render_settings;
    }

    sMapData MapRenderer::SetupMapData() const {
        SALAT_SALTBEGIN();
        const auto stops = transport_catalogue_->GetUnsortedStops();
        if(stops->empty()) {
            SALAT_SALTEND();
            return {0 , 0 , 0 , 0 , 0 , 0 , 0};
        }
        double min_lat = stops->begin()->second.coordinates.lat;
        double max_lat{min_lat};
        double min_lng = stops->begin()->second.coordinates.lng;
        double max_lng{min_lng};
        SALAT_CTMFBEGIN("MINMAX");
        for(const auto& [name, coordinates] : *stops | std::views::values) {
            if(transport_catalogue_->FindStopBusList(name) == nullptr) { continue; }
            min_lat = std::min(min_lat, coordinates.lat);
            min_lng = std::min(min_lng, coordinates.lng);
            max_lat = std::max(max_lat, coordinates.lat);
            max_lng = std::max(max_lng, coordinates.lng);
            SALAT_CTMFBREAK("MINMAX");
        }
        SALAT_CTMFEND("MINMAX");
        constexpr double epsilon  = 1e-6;
        const double     lng_diff = max_lng - min_lng;
        const double     lat_diff = max_lat - min_lat;
        const double     xzmod    = (std::abs(lng_diff) < epsilon)
                                        ? 0.0
                                        : (render_settings_.width - 2 * render_settings_.padding) / lng_diff;
        const double yzmod = (std::abs(lat_diff) < epsilon)
                                 ? 0.0
                                 : (render_settings_.height - 2 * render_settings_.padding) / lat_diff;
        double czmod;
        if(static_cast<bool>(xzmod) && static_cast<bool>(yzmod)) { czmod = std::min(xzmod, yzmod); }
        else { czmod = std::max(xzmod, yzmod); }

        SALAT_SALTEND();
        return {min_lat , min_lng , max_lat , max_lng , xzmod , yzmod , czmod};
    }

    svglib::color_TD MapRenderer::NodeToColor(const jsonlib::Node& arg_node) {
        switch(arg_node.GetValueType()) {
            case jsonlib::DICT_TD : SALAT_RINCTMTTHR(std::invalid_argument
                                                   , "Invalid color palette type, expected "
                                                     "std::string OR sSolidColor OR sTransparentColor, "
                                                     "received dict_TD(a.k.a std::map<std::string , jsonlib::Node>)");
            case jsonlib::BOOL : SALAT_RINCTMTTHR(std::invalid_argument
                                                , "Invalid color palette type, expected "
                                                  "std::string OR sSolidColor OR sTransparentColor, " "received bool");
            case jsonlib::INT : SALAT_RINCTMTTHR(std::invalid_argument
                                               , "Invalid color palette type, expected "
                                                 "std::string OR sSolidColor OR sTransparentColor, " "received int");
            case jsonlib::DOUBLE : SALAT_RINCTMTTHR(std::invalid_argument
                                                  , "Invalid color palette type, expected "
                                                    "std::string OR sSolidColor OR sTransparentColor, "
                                                    "received double");
            case jsonlib::NULLPTR_T : SALAT_RINCTMTTHR(std::invalid_argument
                                                     , "Invalid color palette type, expected "
                                                       "std::string OR sSolidColor OR sTransparentColor, "
                                                       "received NULL");
            case jsonlib::ARRAY_TD : break;
            case jsonlib::STRING : return arg_node.As<std::string>();
        }
        const std::vector<jsonlib::Node>& node = arg_node.As<jsonlib::array_TD>();
        switch(node.size()) {
            case 3 : return svglib::sSolidColor{static_cast<uint8_t>(node.at(0).As<int>())
                                              , static_cast<uint8_t>(node.at(1).As<int>())
                                              , static_cast<uint8_t>(node.at(2).As<int>())};
            case 4 : return svglib::sTransparentColor{static_cast<uint8_t>(node.at(0).As<int>())
                                                    , static_cast<uint8_t>(node.at(1).As<int>())
                                                    , static_cast<uint8_t>(node.at(2).As<int>())
                                                    , static_cast<float>(node.at(3).As<double>())};
            default : SALAT_RINCTMTTHR(std::invalid_argument
                                     , "Invalid array size, expected 3 or 4, received " + std::to_string(node.size()));
        }
    }

    svglib::sPoint MapRenderer::NodeToPoint(const jsonlib::Node& arg_node) {
        const std::vector<jsonlib::Node>& node = arg_node.As<jsonlib::array_TD>();
        if(node.size() != 2) {
            SALAT_RINCTMTTHR(std::invalid_argument
                           , "Invalid array size, expected 2, received " + std::to_string(node.size()));
        }
        return {node.at(0).As<double>() , node.at(1).As<double>()};
    }

    svglib::sPoint MapRenderer::GeoCoordToDrawCoord(const geo::Geo::Coordinates arg_coordinates) const {
        return {(arg_coordinates.lng - map_data_.min_lng) * map_data_.czmod + render_settings_.padding
              , (map_data_.max_lat - arg_coordinates.lat) * map_data_.czmod + render_settings_.padding};
    }

    void MapRenderer::RenderRoutes(std::vector<svglib::Polyline>& arg_routes
                                 , std::vector<svglib::Text>&     arg_routenames) {
        const std::vector<const common::sBus*> buses = transport_catalogue_->GetSortedBuses(true);
        SALAT_BEGIN();
        for(const common::sBus* const bus : buses) {
            svglib::Polyline route;
            route.SetStrokeWidth(render_settings_.line_width).SetStrokeColor(render_settings_.GetNextColor()).
                  SetFillColor("none").SetStrokeLineCap(svglib::eStrokeLineCap::ROUND).
                  SetStrokeLineJoin(svglib::eStrokeLineJoin::ROUND);
            SALAT_CTMFBEGIN("RenderRoute");
            RenderRoute(bus, route, arg_routenames);
            SALAT_CTMFEND("RenderRoute");
            arg_routes.push_back(std::move(route));
            SALAT_BREAK();
        }
        SALAT_END();
    }

    template<bool tIsReverse>
    void MapRenderer::RenderRoute(const common::sBus*        arg_bus
                                , svglib::Polyline&          arg_route
                                , std::vector<svglib::Text>& arg_routenames) {
        if(!arg_bus) { return; }
        auto stops = [](const auto& arg_bus_stops) {
            if constexpr(tIsReverse) { return std::ranges::reverse_view(arg_bus_stops); }
            else { return std::views::all(arg_bus_stops); }
        }(arg_bus->stops);
        auto makelabel = [&](const common::sStop* arg_stop) {
            svglib::Text routenamebg;
            routenamebg.SetFillColor(render_settings_.underlayer_color).
                        SetStrokeWidth(render_settings_.underlayer_width).
                        SetStrokeLineCap(svglib::eStrokeLineCap::ROUND).
                        SetStrokeLineJoin(svglib::eStrokeLineJoin::ROUND).
                        SetStrokeColor(render_settings_.underlayer_color).SetFontFamily("Verdana").SetFontWeight("bold")
                       .SetData(arg_bus->name).SetFontSize(render_settings_.bus_label_font_size).
                        SetOffset(render_settings_.bus_label_offset).
                        SetPosition(GeoCoordToDrawCoord(arg_stop->coordinates));
            arg_routenames.push_back(std::move(routenamebg));
            svglib::Text routenamefg;
            routenamefg.SetFillColor(render_settings_.GetColor()).SetFontFamily("Verdana").SetFontWeight("bold").
                        SetData(arg_bus->name).SetFontSize(render_settings_.bus_label_font_size).
                        SetOffset(render_settings_.bus_label_offset).
                        SetPosition(GeoCoordToDrawCoord(arg_stop->coordinates));
            arg_routenames.push_back(std::move(routenamefg));
        };
        bool first = true;
        for(const common::sStop* stop : stops) {
            if(stop == nullptr) { continue; }
            if(first) {
                first = false;
                if constexpr(tIsReverse) {
                    if(stops.front() != stops.back()) { makelabel(stop); }
                    continue;
                }
                else { makelabel(stop); }
            }
            arg_route.AddPoint(GeoCoordToDrawCoord(stop->coordinates));
            SALAT_BREAK();
        }

        if constexpr(!tIsReverse) {
            if(!arg_bus->is_roundtrip) { RenderRoute<true>(arg_bus, arg_route, arg_routenames); }
        }
    }

    void MapRenderer::RenderStops(std::vector<svglib::Text>&   arg_stopnames
                                , std::vector<svglib::Circle>& arg_stopcircles) const {
        SALAT_BEGIN();
        const std::vector<const common::sStop*> stops = transport_catalogue_->GetSortedStops(true);
        for(const common::sStop* stop : stops) {
            svglib::Circle circle;
            circle.SetFillColor("white").SetCenter(GeoCoordToDrawCoord(stop->coordinates)).
                   SetRadius(render_settings_.stop_radius);
            arg_stopcircles.push_back(std::move(circle));
            svglib::Text stopnamebg;
            stopnamebg.SetPosition(GeoCoordToDrawCoord(stop->coordinates)).SetOffset(render_settings_.stop_label_offset)
                      .SetFontSize(render_settings_.stop_label_font_size).SetFontFamily("Verdana").SetData(stop->name).
                       SetFillColor(render_settings_.underlayer_color).SetStrokeWidth(render_settings_.underlayer_width)
                      .SetStrokeLineCap(svglib::eStrokeLineCap::ROUND).SetStrokeColor(render_settings_.underlayer_color)
                      .SetStrokeLineJoin(svglib::eStrokeLineJoin::ROUND);
            arg_stopnames.push_back(std::move(stopnamebg));
            svglib::Text stopnamefg;
            stopnamefg.SetPosition(GeoCoordToDrawCoord(stop->coordinates)).SetOffset(render_settings_.stop_label_offset)
                      .SetFontSize(render_settings_.stop_label_font_size).SetFontFamily("Verdana").SetData(stop->name).
                       SetFillColor("black");
            arg_stopnames.push_back(std::move(stopnamefg));
            SALAT_BREAK();
        }
        SALAT_END();
    }
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
