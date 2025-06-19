#include "transport_catalogue.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <unordered_set>

void TransportCatalogue::AddStop(const std::string_view name, const Coordinates& coords) {
    ///try_emplace тут ломает все, остановки же создаются при создании
    ///автобусов, чтобы потом в автобус можно было добавить указатели,
    ///а тут оно либо создается, либо меняется, а не только создается
    stops_[std::string(name)] = std::move(Stop{std::string(name), coords});
}

void TransportCatalogue::AddBus(const std::string_view name, const std::vector<std::string_view>& stop_names, const bool is_roundtrip) {
    std::string name_string = std::string(name);
    std::vector<const Stop*> stop_ptrs;
    for (auto stop_name : stop_names) {
        std::string stop_name_string = std::string(stop_name);
        buses_by_stop_[stop_name_string].insert(name_string);
        const Stop *it = &stops_[stop_name_string];;
        stop_ptrs.push_back(it);
    }
    buses_.try_emplace(name_string, std::move(name_string), std::move(stop_ptrs), is_roundtrip);
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

const std::set<std::string> *TransportCatalogue::FindStopBusList(const std::string_view name) const {
    return FindTemplate(buses_by_stop_, name);
}

size_t TransportCatalogue::GetBusStopCount(const std::string_view name) const {
    const Bus* bus = FindBus(name);
    return bus ? bus->stops.size() : 0;
}

std::unique_ptr<std::tuple<size_t, size_t, std::string>> TransportCatalogue::GetBusStats(const Bus *bus) const {
    double distance = 0;
    if (!bus->stops.empty()) {
        const Stop *prevstop = bus->stops.front();
        for (const Stop *stop: bus->stops) {
            distance+= ComputeDistance(prevstop->coordinates, stop->coordinates);
            prevstop = stop;
        }
    }
    std::ostringstream oss;
    oss << std::setprecision(6) << distance;
    return std::make_unique<std::tuple<size_t, size_t, std::string>>(
        std::tuple{GetBusStopCount(bus->name), GetUniqueStopCount(bus->name), oss.str()});
}

size_t TransportCatalogue::GetUniqueStopCount(const std::string_view name) const {
    const Bus* bus = FindBus(name);
    if (!bus) {
        return 0;
    }
    std::unordered_set<std::string_view> unique_stops;
    for (const Stop* stop : bus->stops) {
        unique_stops.insert(stop->name);
    }
    return unique_stops.size();
}

std::string TransportCatalogue::GetBusListAsString(const std::string_view name) const {
    const std::set<std::string>* buslist = FindStopBusList(name);
    std::string out;
    if (buslist == nullptr) { return std::move(out);}
    std::ostringstream ous;
    for (const std::string& bus: *buslist) {
        ous << bus << " ";
    }
    out = ous.str();
    out.pop_back();
    return std::move(out);
}


