#pragma once

#include <iosfwd>
#include <string_view>
#include <iomanip>
#include <iostream>
#include "transport_catalogue.h"

class Parser {
public:
    static void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
private:
    struct BusStatsToStream {
        const BusStats& bus_stats;
        friend  std::ostream& operator<<(std::ostream& os, const BusStatsToStream& bsts) {
            os  << bsts.bus_stats.stop_count
                << " stops on route, "
                << bsts.bus_stats.unique_stop_count
                << " unique stops, "
                << std::setprecision(6) << static_cast<double>(bsts.bus_stats.factual_distance)
                << " route length, "
                << bsts.bus_stats.curvature
                << " curvature\n";
            return os;
        }
    };
    struct BusListToStream {
        const std::set<std::string>* bus_list;
        friend  std::ostream& operator<<(std::ostream& os, const BusListToStream& blts) {
            if (!blts.bus_list) { return os; }
            auto it = blts.bus_list->begin();
            os << "buses " << *it;
            ++it;
            for (; it != blts.bus_list->end(); ++it) {
                os << ' ' << *it;
            }
            return os;
        }
    };
};




