#pragma once
#include <functional>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

namespace transport_catalogue::router {
    class TransportRouter {
        struct sEdgeData {
            std::variant<const common::sStop* , const common::sBus*> data;
            double                                                   weight{};
            std::optional<size_t>                                    span_count;
        };
        std::unordered_map<size_t , sEdgeData>                get_edge_by_id_;
        std::unordered_map<const common::sStop* , size_t>     get_stop_beg_id_;
        std::unordered_map<const common::sStop* , size_t>     get_stop_end_id_;
        std::unique_ptr<graph::DirectedWeightedGraph<double>> maingraph_;
        std::unique_ptr<graph::Router<double>>                mainrouter_;

        static double CalculateTimeMinutes(const double arg_distance_meters , const double arg_speed_kmh) {
            const double speed_m_per_min = arg_speed_kmh * 1000.0 / 60.0;
            return arg_distance_meters / speed_m_per_min;
        }
        public:
            TransportRouter() = delete;
            TransportRouter(TransportRouter&&) = delete;
            TransportRouter(const TransportRouter&) = delete;
            TransportRouter(const TransportCatalogue*       arg_transport_catalogue
                          , const common::sRoutingSettings& arg_routing_settings);
            std::optional<common::sRouteInfo> GetRoute(const common::sStop* arg_from
                                                     , const common::sStop* arg_to) const;
    };
}
