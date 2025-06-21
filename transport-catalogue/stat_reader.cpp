#include "stat_reader.h"

void Parser::ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    const std::size_t space_pos = request.find(' ');
    const std::string_view type = request.substr(0, space_pos);
    const std::string_view id = request.substr(space_pos + 1);
    if (type == "Bus") {
        output << "Bus " << id << ": ";
        if (const Bus *bus = transport_catalogue.FindBus(id); !bus) {
            output << "not found";
        } else {
            output << BusStatsToStream{BusStats{bus, transport_catalogue}};
        }
    } else if (type == "Stop") {
        output << "Stop " << id << ": ";
        if (const Stop *stop = transport_catalogue.FindStop(id); !stop) {
            output << "not found";
        } else {
            if (const std::set<std::string>* bus_list = transport_catalogue.FindStopBusList(stop->name); bus_list == nullptr) {
                output << "no buses" ;
            } else {
                output << BusListToStream{bus_list};
            }
        }
    }
    output << "\n";
}
