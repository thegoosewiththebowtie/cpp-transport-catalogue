/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#include "transport_catalogue.h"

#include <algorithm>
#include <ranges>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "common.h"
#include "geo.h"
namespace transport_catalogue {
    /*[BEGIN:========================================TRANSPORTCATALOGUE==============================================]*/
    /*[BEGIN:========================================TRANSPORTCATALOGUE:ADDERS=======================================]*/
    void TransportCatalogue::AddStop(const std::string_view arg_name , const geo::Geo::Coordinates& arg_coords) {
        stops_[std::string(arg_name)] = std::move(common::sStop{std::string(arg_name) , arg_coords});
    }

    void TransportCatalogue::AddBus(const std::string_view               arg_name
                                  , const std::vector<std::string_view>& arg_stop_names
                                  , const bool                           arg_is_roundtrip) {
        std::string                 name_string = std::string(arg_name);
        std::vector<common::sStop*> stop_ptrs;
        for(auto stop_name : arg_stop_names) {
            std::string stop_name_string = std::string(stop_name);
            (void)buses_by_stop_[stop_name_string].insert(name_string);
            common::sStop* it = &stops_[stop_name_string];
            stop_ptrs.push_back(it);
        }
        buses_[std::move(name_string)] = std::move(common::sBus{name_string , std::move(stop_ptrs) , arg_is_roundtrip});
    }

    void TransportCatalogue::AddStopsRoute(const std::string_view arg_name1
                                         , const std::string_view arg_name2
                                         , const int              arg_distance) {
        common::sStop* stop1            = &stops_[std::string(arg_name1)];
        common::sStop* stop2            = &stops_[std::string(arg_name2)];
        road_distance_[{stop1 , stop2}] = arg_distance;
    }

    /*[END:==========================================TRANSPORTCATALOGUE:ADDERS=======================================]*/

    /*[BEGIN:========================================TRANSPORTCATALOGUE:FINDERS======================================]*/
    template<typename tContainer>
    auto gFindTemplate(const tContainer&      arg_container
                     , const std::string_view arg_name) -> decltype(&arg_container.begin()->second) {
        auto iter = arg_container.find(std::string(arg_name));
        return iter == arg_container.end() ? nullptr : &iter->second;
    }

    const common::sStop* TransportCatalogue::FindStop(const std::string_view arg_name) const {
        return gFindTemplate(stops_, arg_name);
    }

    const common::sBus* TransportCatalogue::FindBus(const std::string_view arg_name) const {
        return gFindTemplate(buses_, arg_name);
    }

    const std::set<std::string>* TransportCatalogue::FindStopBusList(const std::string_view arg_name) const {
        return gFindTemplate(buses_by_stop_, arg_name);
    }

    /*[END:==========================================TRANSPORTCATALOGUE:FINDERS======================================]*/

    /*[BEGIN:========================================TRANSPORTCATALOGUE:GETTERS======================================]*/
    common::sBusStats TransportCatalogue::GetBusStats(const common::sBus* arg_bus) const {
        const double factual_distance = GetRouteFactualDistance(arg_bus);
        return {GetBusStopCount(arg_bus)
              , GetUniqueStopCount(arg_bus)
              , factual_distance
              , geo::Geo::GetRouteCurvature(factual_distance, GetRouteGeoDistance(arg_bus))};
    }

    size_t TransportCatalogue::GetBusStopCount(const common::sBus* arg_bus) {
        const size_t count = arg_bus->stops.size();
        return arg_bus->is_roundtrip ? count : 2 * count - 1;
    }

    size_t TransportCatalogue::GetUniqueStopCount(const common::sBus* arg_bus) {
        if(!arg_bus) { return 0; }
        std::unordered_set<std::string_view> unique_stops;
        for(const common::sStop* stop : arg_bus->stops) { (void)unique_stops.insert(stop->name); }
        return unique_stops.size();
    }

    double TransportCatalogue::GetRouteGeoDistance(const common::sBus* arg_bus) {
        double distance = 0.;
        if(!arg_bus->stops.empty()) {
            const common::sStop* prevstop = arg_bus->stops.front();
            for(const common::sStop* stop : arg_bus->stops) {
                distance += geo::Geo::ComputeDistance(prevstop->coordinates, stop->coordinates);
                prevstop = stop;
            }
            if(!arg_bus->is_roundtrip) {
                for(const common::sStop* stop : std::ranges::reverse_view(arg_bus->stops)) {
                    distance += geo::Geo::ComputeDistance(prevstop->coordinates, stop->coordinates);
                    prevstop = stop;
                }
            }
        }
        return distance;
    }

    std::vector<std::string_view> TransportCatalogue::GetSortedBusesView(const bool arg_onlywithstops) const {
        std::vector<std::string_view> retbuses;
        retbuses.reserve(buses_.size());
        for(const auto& [name, bus] : buses_) {
            if(arg_onlywithstops && bus.stops.empty()) { continue; }
            retbuses.emplace_back(name);
        }
        std::sort(retbuses.begin(), retbuses.end());
        return retbuses;
    }

    std::vector<const common::sBus*> TransportCatalogue::GetSortedBuses(const bool arg_onlywithstops) const {
        std::vector<const common::sBus*> retbuses;
        retbuses.reserve(buses_.size());
        for(const common::sBus& bus : buses_ | std::views::values) {
            if(arg_onlywithstops && bus.stops.empty()) { continue; }
            retbuses.emplace_back(&bus);
        }
        std::sort(retbuses.begin()
                , retbuses.end()
                , [](const common::sBus* arg_bus1 , const common::sBus* arg_bus2) {
                      return arg_bus1->name < arg_bus2->name;
                  });
        return retbuses;
    }

    std::vector<std::string_view> TransportCatalogue::GetSortedStopsView(const bool arg_onlywithbuses) const {
        std::vector<std::string_view> retstops;
        retstops.reserve(stops_.size());
        for(const auto& name : stops_ | std::views::keys) {
            if(arg_onlywithbuses && FindStopBusList(name) == nullptr) { continue; }
            retstops.emplace_back(name);
        }
        std::sort(retstops.begin(), retstops.end());
        return retstops;
    }

    std::vector<const common::sStop*> TransportCatalogue::GetSortedStops(const bool arg_onlywithbuses) const {
        std::vector<const common::sStop*> retstops;
        retstops.reserve(stops_.size());
        for(const common::sStop& stop : stops_ | std::views::values) {
            if(arg_onlywithbuses && FindStopBusList(stop.name) == nullptr) { continue; }
            retstops.emplace_back(&stop);
        }
        std::sort(retstops.begin()
                , retstops.end()
                , [](const common::sStop* arg_stop1 , const common::sStop* arg_stop2) {
                      return arg_stop1->name < arg_stop2->name;
                  });
        return retstops;
    }

    const std::unordered_map<std::string , common::sBus>* TransportCatalogue::GetUnsortedBuses() const {
        return &buses_;
    }

    const std::unordered_map<std::string , common::sStop>* TransportCatalogue::GetUnsortedStops() const {
        return &stops_;
    }

    double TransportCatalogue::GetStopsDistance(common::sStop* arg_stop1 , common::sStop* arg_stop2) const {
        return road_distance_.find({arg_stop1 , arg_stop2}) != road_distance_.end()
                   ? road_distance_.at({arg_stop1 , arg_stop2})
                   : road_distance_.find({arg_stop2 , arg_stop1}) != road_distance_.end()
                         ? road_distance_.at({arg_stop2 , arg_stop1})
                         : 0.0;
    }

    double TransportCatalogue::GetRouteFactualDistance(const common::sBus* arg_bus) const {
        double distance = 0;
        for(size_t i = 1 ; i < arg_bus->stops.size() ; ++i) {
            distance += GetStopsDistance(arg_bus->stops[i - 1], arg_bus->stops[i]);
        }
        if(!arg_bus->is_roundtrip) {
            for(size_t i = arg_bus->stops.size() - 1 ; i > 0 ; --i) {
                distance += GetStopsDistance(arg_bus->stops[i], arg_bus->stops[i - 1]);
            }
        }
        return distance;
    }

    /*[END:==========================================TRANSPORTCATALOGUE:GETTERS======================================]*/
    /*[END:==========================================TRANSPORTCATALOGUE==============================================]*/
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
