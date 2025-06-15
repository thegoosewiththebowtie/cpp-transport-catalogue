#include "stat_reader.h"

#include <algorithm>
#include <format>
#include <iomanip>
#include <iostream>

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output) {
    auto space_pos = request.find(' ');
    std::string_view type = request.substr(0, space_pos);
    std::string_view id = request.substr(space_pos + 1);
    if (type == "Bus") {
        if (const auto* bus = tansport_catalogue.FindBus(id); !bus) {
            output << "Bus " << id << ": not found\n";
        } else {
            const size_t stop_count = tansport_catalogue.GetBusStopCount(id);
            const size_t unique_stop_count = tansport_catalogue.GetUniqueStopCount(id);
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
            output << "Bus " << id << ": " << stop_count << " stops on route, "
                   << unique_stop_count << " unique stops, " << oss.str() << " route length\n";
        }
    } else if (type == "Stop") {
        auto getbuslist = [](const  std::unordered_map<std::string, const Bus*>& buslist) {
            std::string out;
            std::vector<std::string> busnames;
            for (const std::pair<const std::string, const Bus *>& bus: buslist) {
                busnames.push_back(bus.first);
            }
            std::sort(busnames.begin(), busnames.end());
            for (std::string bus: busnames) {
                out += std::move(bus) + " ";
            }
            out.pop_back();
            return out;
        };
        if (const Stop *stop = tansport_catalogue.FindStop(id); !stop) {
            output << "Stop " << id << ": not found\n";
        } else {
            output << "Stop " << id << (tansport_catalogue.GetBusList(stop->name) == nullptr ?
                                        ": no buses\n" :
                                        ": buses " + getbuslist(*tansport_catalogue.GetBusList(stop->name)) + "\n");
        }
    }
}
