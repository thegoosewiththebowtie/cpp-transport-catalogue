#include "stat_reader.h"

#include <algorithm>
#include <format>
#include <iomanip>
#include <iostream>

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    auto space_pos = request.find(' ');
    const std::string_view type = request.substr(0, space_pos);
    const std::string_view id = request.substr(space_pos + 1);
    if (type == "Bus") {
        if (const Bus *bus = transport_catalogue.FindBus(id); !bus) {
            output << "Bus " << id << ": not found\n";
        } else {
            std::unique_ptr<std::tuple<size_t, size_t, std::string>> bus_stats = transport_catalogue.GetBusStats(bus);
            output << "Bus " << id << ": " << std::get<0>(*bus_stats) << " stops on route, "
                   << std::get<1>(*bus_stats) << " unique stops, " << std::get<2>(*bus_stats) << " route length\n";
        }
    } else if (type == "Stop") {
        const Stop *stop = transport_catalogue.FindStop(id);
        output << "Stop " << id << (!stop ?
            ": not found\n"
            : (transport_catalogue.FindStopBusList(stop->name) == nullptr ?
            ": no buses\n"
            : ": buses " + transport_catalogue.GetBusListAsString(stop->name) + "\n"));
    }
}
