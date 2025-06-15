#include "transport_catalogue.h"
#include <iostream>
#include <unordered_set>

void TransportCatalogue::AddStop(std::string_view name, Coordinates coords) {
    stops_[std::string(name)] = std::move(Stop{std::string(name), coords});
}

void TransportCatalogue::AddBus(std::string_view name, const std::vector<std::string_view>& stop_names, bool is_roundtrip) {
    const std::string name_string = std::string(name);
    std::vector<const Stop*> stop_ptrs;
    buses_[name_string];
    for (auto stop_name : stop_names) {
        std::string stop_name_string = std::string(stop_name);
        buses_by_stop[stop_name_string][name_string] = (FindBus(name));
        const Stop *it = &stops_[stop_name_string];;
        stop_ptrs.push_back(it);
    }
    buses_[name_string]= std::move(Bus(name_string,stop_ptrs, is_roundtrip));
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {
    const std::string name_string = std::string(name);
    return stops_.contains(name_string) ? &stops_.at(name_string) : nullptr;
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    const std::string name_string = std::string(name);
    return buses_.contains(name_string)? &buses_.at(name_string) : nullptr;
}

size_t TransportCatalogue::GetBusStopCount(std::string_view bus_name) const {
    const Bus* bus = FindBus(bus_name);
    return bus ? bus->stops.size() : 0;
}

size_t TransportCatalogue::GetUniqueStopCount(std::string_view bus_name) const {
    const Bus* bus = FindBus(bus_name);
    if (!bus) {
        return 0;
    }
    std::unordered_map<std::string_view, Coordinates> unique_stops;
    for (const Stop* stop : bus->stops) {
        if (unique_stops[stop->name] == stop->coordinates) {
            continue;
        }
        unique_stops[stop->name] = stop->coordinates;
    }
    return unique_stops.size();
}

const std::unordered_map<std::string, const Bus *>* TransportCatalogue::GetBusList(std::string_view stop_name) const {
    return buses_by_stop.contains(std::string(stop_name))? &buses_by_stop.at(std::string(stop_name)) : nullptr;
}


