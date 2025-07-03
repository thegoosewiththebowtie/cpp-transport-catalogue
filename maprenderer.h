/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <map>
#include <unordered_map>
#include <utility>

#include "common.h"
#include "geo.h"
#include "svgLib/svglib.h"
namespace transport_catalogue {
    class TransportCatalogue;
    namespace common {
        struct sStop;
    }}
namespace jsonlib {
    class Node;
}
namespace transport_catalogue::map_renderer {
    struct sRenderSettings {
        sRenderSettings() = default;

        sRenderSettings(const double          arg_width
                      , const double          arg_height
                      , const double          arg_padding
                      , const double          arg_line_width
                      , const double          arg_stop_radius
                      , const unsigned        arg_bus_label_font_size
                      , const svglib::sPoint& arg_bus_label_offset
                      , const unsigned        arg_stop_label_font_size
                      , const svglib::sPoint& arg_stop_label_offset
                      , svglib::color_TD      arg_underlayer_color
                      , const double          arg_underlayer_width)
            : width(arg_width)
          , height(arg_height)
          , padding(arg_padding)
          , line_width(arg_line_width)
          , stop_radius(arg_stop_radius)
          , underlayer_width(arg_underlayer_width)
          , bus_label_font_size(arg_bus_label_font_size)
          , stop_label_font_size(arg_stop_label_font_size)
          , bus_label_offset(arg_bus_label_offset)
          , stop_label_offset(arg_stop_label_offset)
          , underlayer_color(std::move(arg_underlayer_color)) {}

        void AddColor(svglib::color_TD&& arg_colour) { colour_palette_.push_back(std::move(arg_colour)); }

        [[nodiscard]] const svglib::color_TD& GetNextColor() {
            if(colour_palette_.empty()) { return kNull; }
            if(current_id_ == colour_palette_.size() - 1) { current_id_ = 0; }
            else { ++current_id_; }
            return colour_palette_[current_id_ == 0 ? colour_palette_.size() - 1 : current_id_ - 1];
        }

        [[nodiscard]] const svglib::color_TD& GetColor() const {
            if(colour_palette_.empty()) { return kNull; }
            return colour_palette_[current_id_ == 0 ? colour_palette_.size() - 1 : current_id_ - 1];
        }

        double           width{} , height{} , padding{} , line_width{} , stop_radius{} , underlayer_width{};
        unsigned         bus_label_font_size{} , stop_label_font_size{};
        svglib::sPoint   bus_label_offset{} , stop_label_offset{};
        svglib::color_TD underlayer_color{};
        private:
            unsigned long long                current_id_{0};
            static constexpr svglib::color_TD kNull{};
            std::vector<svglib::color_TD>     colour_palette_;
    };
    struct sMapData {
        double min_lat , min_lng , max_lat , max_lng , xzmod , yzmod , czmod;
    };
    class MapRenderer {
        public:
            MapRenderer(const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue
                      , const std::map<std::string , jsonlib::Node>&
                        arg_render_settings) : transport_catalogue_(arg_transport_catalogue.get())
                                           , render_settings_(SetupRenderSettings(arg_render_settings))
                                           , map_data_{SetupMapData()} {}

            void Render(std::ostream& arg_output);
        private:
            [[nodiscard]] static sRenderSettings SetupRenderSettings(
                    const std::map<std::string , jsonlib::Node>& arg_render_settings);
            [[nodiscard]] sMapData SetupMapData() const;
            [[nodiscard]] static svglib::color_TD NodeToColor(const jsonlib::Node& arg_node);
            [[nodiscard]] static svglib::sPoint NodeToPoint(const jsonlib::Node& arg_node);
            [[nodiscard]] svglib::sPoint GeoCoordToDrawCoord(geo::Geo::Coordinates arg_coordinates) const;
            void RenderRoutes(std::vector<svglib::Polyline>& arg_routes , std::vector<svglib::Text>& arg_routenames);
            template<bool tIsReverse = false>
            void RenderRoute(const common::sBus* arg_bus
                           , svglib::Polyline& arg_route
                           , std::vector<svglib::Text>& arg_routenames);
            void                RenderStops(std::vector<svglib::Text>& arg_stopnames , std::vector<svglib::Circle>& arg_stopcircles) const;
            TransportCatalogue* transport_catalogue_;
            sRenderSettings     render_settings_;
            sMapData            map_data_;
    };
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
