#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"
class Parcer {
public:
    static void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request, std::ostream& output);
private:
    static std::string BusListToString(const std::set<std::string>* buslist);

    template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
    static std::string FloatToString(T num, int precision);
};
