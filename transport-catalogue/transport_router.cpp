#include "transport_router.h"

#include <deque>
#include <ranges>

namespace transport_catalogue::router {
    TransportRouter::TransportRouter(const TransportCatalogue* arg_transport_catalogue
                                   , const common::sRoutingSettings& arg_routing_settings) {
        const std::vector<const common::sStop*> all_stops = arg_transport_catalogue->GetSortedStops(true);
        maingraph_ = std::make_unique<graph::DirectedWeightedGraph<double>>(all_stops.size() * 2);
        size_t id = 0;
        get_stop_end_id_.reserve(all_stops.size());
        get_stop_beg_id_.reserve(all_stops.size());
        for(const common::sStop* cur_stop : all_stops) {
            get_stop_end_id_[cur_stop] = id++;
            get_stop_beg_id_[cur_stop] = id++;
            size_t mgeid               = maingraph_->AddEdge({get_stop_end_id_.at(cur_stop)
                                                            , get_stop_beg_id_.at(cur_stop)
                                                            , static_cast<double>(arg_routing_settings.bus_wait_time)});
            get_edge_by_id_[mgeid] = {cur_stop
                                    , static_cast<double>(arg_routing_settings.bus_wait_time)
                                    , std::nullopt};
        }
        const auto buses = arg_transport_catalogue->GetSortedBuses(true);
        for(const common::sBus* bus : buses) {
            const std::vector<common::sStop*>& stops = bus->stops;
            for(size_t i = 0 ; i < stops.size() ; ++i) {
                double time_sum = 0.0;
                for(size_t j = i + 1 ; j < stops.size() ; ++j) {
                    time_sum += CalculateTimeMinutes(arg_transport_catalogue->GetStopsDistance(stops[j - 1], stops[j])
                                                   , arg_routing_settings.bus_velocity);

                    const size_t edge_id = maingraph_->AddEdge({get_stop_beg_id_.at(stops[i])
                                                              , get_stop_end_id_.at(stops[j])
                                                              , time_sum});
                    get_edge_by_id_[edge_id] = {bus , time_sum , j - i};
                }
            }
            if(!bus->is_roundtrip) {
                auto reversed_stops = std::ranges::reverse_view(bus->stops);
                for(int i = 0 ; i < stops.size() ; ++i) {
                    double time_sum = 0.0;
                    for(int j = i + 1 ; j < stops.size() ; ++j) {
                        time_sum += CalculateTimeMinutes(arg_transport_catalogue->
                                                         GetStopsDistance(reversed_stops[j - 1], reversed_stops[j])
                                                       , arg_routing_settings.bus_velocity);

                        const size_t edge_id = maingraph_->AddEdge({get_stop_beg_id_.at(reversed_stops[i])
                                                                  , get_stop_end_id_.at(reversed_stops[j])
                                                                  , time_sum});
                        get_edge_by_id_[edge_id] = {bus , time_sum , j - i};
                    }
                }
            }
        }
        mainrouter_ = std::make_unique<graph::Router<double>>(*maingraph_);
    }

    std::optional<common::sRouteInfo> TransportRouter::GetRoute(const common::sStop* arg_from
                                                              , const common::sStop* arg_to) const {
        if(!get_stop_end_id_.contains(arg_from) || !get_stop_end_id_.contains(arg_to)) { return std::nullopt; }
        const size_t                                          from_stop_id = get_stop_end_id_.at(arg_from);
        const size_t                                          to_stop_id = get_stop_end_id_.at(arg_to);
        const std::optional<graph::Router<double>::RouteInfo> route = mainrouter_->BuildRoute(from_stop_id, to_stop_id);
        if(!route.has_value()) { return std::nullopt; }
        common::sRouteInfo                ret_route_info;
        double                            time_sum = 0.0;
        const std::vector<graph::EdgeId>& edges    = route.value().edges;
        ret_route_info.items.reserve(edges.size());
        for(size_t value : edges) {
            auto [data, time, span_count] = get_edge_by_id_.at(value);
            time_sum += time;
            if(std::holds_alternative<const common::sStop*>(data)) {
                const common::sStop* stop = std::get<const common::sStop*>(data);
                ret_route_info.items.emplace_back(common::sRouteItem{stop->name , false , time});
            }
            else if(std::holds_alternative<const common::sBus*>(data)) {
                const common::sBus* bus = std::get<const common::sBus*>(data);
                ret_route_info.items.emplace_back(common::sRouteItem{bus->name , true , time , span_count});
            }
        }
        ret_route_info.total_time = time_sum;
        return ret_route_info;
    }
}
