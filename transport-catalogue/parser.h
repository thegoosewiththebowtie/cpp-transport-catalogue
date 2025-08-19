/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <iomanip>
#include <iosfwd>
#include <map>
#include <vector>

#include "domain.h"
#include "transport_router.h"
namespace json {
    class Builder;
    class Node;
}
namespace transport_catalogue {
    class TransportCatalogue;
    namespace parser {
        class Parser {
            public:
                Parser(TransportCatalogue* arg_transport_catalogue) : transport_catalogue_(arg_transport_catalogue) {};
                void ChangeCatalogue(TransportCatalogue* arg_transport_catalogue);
                void Deserialize(std::istream& arg_istream);
                void RouteToNode(int                      arg_id
                               , const std::string&       arg_from
                               , const std::string&       arg_to
                               , json::Builder&           arg_builder
                               , const router::TransportRouter& arg_router) const;
                void Serialize(std::ostream& arg_output , common::eOutputFilter);
            private:
                void AddNodeBus(const std::map<std::string , json::Node>& arg_busmap) const;
                void AddNodeStop(const std::map<std::string , json::Node>& arg_stopmap) const;
                void SetRenderSettingsFromNode(const std::map<std::string , json::Node>& arg_render_settings);
                void SetRoutingSettingsFromNode(const std::map<std::string , json::Node>& arg_routing_settings);
                void BusToNode(int arg_id , const std::string& arg_bus_name , json::Builder& arg_builder) const;
                void StopToNode(int arg_id , const std::string& arg_stop_name , json::Builder& arg_builder) const;
                void GetSvgNode(int arg_id , json::Builder& arg_builder);
                void Render(std::ostream& arg_output);
                [[nodiscard]] static svglib::color_TD NodeToColor(const json::Node& arg_node);
                [[nodiscard]] static svglib::sPoint NodeToPoint(const json::Node& arg_node);
                void RenderRoutes(std::vector<svglib::Polyline>& arg_routes
                                , std::vector<svglib::Text>&     arg_routenames);
                void RouteToPolyline(const common::sBus*        arg_bus
                                   , svglib::Polyline&          arg_route
                                   , std::vector<svglib::Text>& arg_routenames) const;
                void RenderStops(std::vector<svglib::Text>&   arg_stopnames
                               , std::vector<svglib::Circle>& arg_stopcircles) const;
                TransportCatalogue* transport_catalogue_;
                common::sMapData map_data_{};
                common::sRenderSettings  render_settings_{};
                common::sRoutingSettings routing_settings_{};
                common::sRequests requests_{};
        };
    }}

/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
