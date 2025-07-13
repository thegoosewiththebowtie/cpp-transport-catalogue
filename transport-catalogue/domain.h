/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#define SALAT_ENABLED 1
#include <map>
#include <string>
#include <vector>

#include "json.h"
#include "svg.h"

namespace transport_catalogue :: common {
    /*[BEGIN:==============================================STRUCTS===================================================]*/
    enum class eOutputFilter{NONE, MAP, REQUESTS};
    struct Coordinates {
        double lat;
        double lng;

        bool operator==(const Coordinates& arg_other) const {
            return lat == arg_other.lat && lng == arg_other.lng;
        }

        bool operator!=(const Coordinates& arg_other) const { return !(*this == arg_other); }
    };
    struct sStop {
        std::string           name;
        Coordinates coordinates;
    };
    struct sBus {
        std::string         name;
        std::vector<sStop*> stops;
        bool                is_roundtrip{};
    };
    struct sBusStats {
        size_t stop_count;
        size_t unique_stop_count;
        double factual_distance;
        double curvature;
    };
    struct sRenderSettings {
        void AddColor(svglib::color_TD&& arg_colour) { color_palette_.push_back(std::move(arg_colour)); }

        [[nodiscard]] const svglib::color_TD& GetNextColor() {
            if(color_palette_.empty()) { return kNull; }
            if(current_id_ == color_palette_.size() - 1) { current_id_ = 0; }
            else { ++current_id_; }
            return color_palette_[current_id_ == 0 ? color_palette_.size() - 1 : current_id_ - 1];
        }

        [[nodiscard]] const svglib::color_TD& GetColor() const {
            if(color_palette_.empty()) { return kNull; }
            return color_palette_[current_id_ == 0 ? color_palette_.size() - 1 : current_id_ - 1];
        }
        double           width{} , height{} , padding{} , line_width{} , stop_radius{} , underlayer_width{};
        int  bus_label_font_size{} , stop_label_font_size{};
        svglib::sPoint   bus_label_offset{} , stop_label_offset{};
        svglib::color_TD underlayer_color{};
        private:
            unsigned long long current_id_{0};
            static constexpr svglib::color_TD kNull{};
            std::vector<svglib::color_TD>     color_palette_{};
    };

    struct sMapData {
        double min_lat , min_lng , max_lat , max_lng , xzmod , yzmod , czmod;
    };
    struct sRequests {
        std::map<std::string , json::Node>  main_map{};
        std::vector<json::Node>*            input_requests{};
        std::map<std::string , json::Node>* render_settings{};
        std::vector<json::Node>*            output_requests{};
    };
    /*[END:================================================STRUCTS===================================================]*/
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
