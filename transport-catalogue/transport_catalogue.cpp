#include "transport_catalogue.h"

#include <iomanip>
#include <unordered_set>

void TransportCatalogue::AddStop(const std::string_view name, const Geo::Coordinates& coords) {
    stops_[std::string(name)] = std::move(Stop{std::string(name), coords});
}

void TransportCatalogue::AddBus(const std::string_view name, const std::vector<std::string_view>& stop_names, const bool is_roundtrip) {
    std::string name_string = std::string(name);
    std::vector<Stop*> stop_ptrs;
    for (auto stop_name : stop_names) {
        std::string stop_name_string = std::string(stop_name);
        buses_by_stop_[stop_name_string].insert(name_string);
        Stop *it = &stops_[stop_name_string];;
        stop_ptrs.push_back(it);
    }
    buses_[std::move(name_string)] = std::move(Bus{name_string, std::move(stop_ptrs), is_roundtrip});
}

void TransportCatalogue::AddStopsRoute(std::string_view name1, std::string_view name2, int distance) {
    Stop* stop1 = &stops_[std::string(name1)];
    Stop* stop2 = &stops_[std::string(name2)];
    road_distance_[{stop1, stop2}] = distance;
}


template <typename Container>
auto FindTemplate(const Container& container, const std::string_view name) -> decltype(&container.begin()->second) {
    auto iter = container.find(std::string(name));
    return iter == container.end() ? nullptr : &iter->second;
}

const Stop* TransportCatalogue::FindStop(const std::string_view name) const {
   return FindTemplate(stops_, name);
}

const Bus* TransportCatalogue::FindBus(const std::string_view name) const {
    return FindTemplate(buses_, name);
}

const std::set<std::string>* TransportCatalogue::FindStopBusList(const std::string_view name) const {
    return FindTemplate(buses_by_stop_, name);
}

BusStats TransportCatalogue::GetBusStats(const Bus *bus) const {
    return {bus, *this};
}

size_t TransportCatalogue::GetBusStopCount(const Bus *bus) {
    return bus ? bus->stops.size() : 0;
}

size_t TransportCatalogue::GetUniqueStopCount(const Bus *bus) {
    if (!bus) {
        return 0;
    }
    std::unordered_set<std::string_view> unique_stops;
    for (const Stop* stop : bus->stops) {
        unique_stops.insert(stop->name);
    }
    return unique_stops.size();
}

int TransportCatalogue::GetStopsDistance(Stop* stop1, Stop* stop2) const {
    return road_distance_.find({stop1, stop2}) != road_distance_.end()?
               road_distance_.at({stop1, stop2}) :
               road_distance_.find({stop2, stop1}) != road_distance_.end()?
                   road_distance_.at({stop2, stop1}) :
                   0;
}

double TransportCatalogue::GetRouteGeoDistance(const Bus *bus) {
    double distance = 0;
    if (!bus->stops.empty()) {
        const Stop *prevstop = bus->stops.front();
        for (const Stop *stop: bus->stops) {
            distance += Geo::ComputeDistance(prevstop->coordinates, stop->coordinates);
            prevstop = stop;
        }
    }
    return distance;
}

int TransportCatalogue::GetRouteFactualDistance(const Bus *bus) const {
    int distance = 0;
    if (!bus->stops.empty()) {
        Stop *prevstop = bus->stops.front();
        for (Stop *stop: bus->stops) {
            distance += GetStopsDistance(prevstop, stop);
            prevstop = stop;
        }
    }
    return distance;
}


