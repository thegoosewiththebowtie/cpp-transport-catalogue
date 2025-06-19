#include "stat_reader.h"

#include <algorithm>
#include <format>
#include <iomanip>
#include <iostream>

void Parcer::ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    auto space_pos = request.find(' ');
    const std::string_view type = request.substr(0, space_pos);
    const std::string_view id = request.substr(space_pos + 1);
    if (type == "Bus") {
        if (const Bus *bus = transport_catalogue.FindBus(id); !bus) {
            output << "Bus " << id << ": not found\n";
        } else {
            /// >>Да зачем здесь целый unique_ptr?
            /// ну так он тут потому что я невероятно тупая
            /// и стул, на котором я сейчас сижу, обладает бОльшим интеллектом,
            /// чем существо, которое сейчас сидит перед клавиатурой
            /// и должно было быть отчислено без возврата средств
            /// уже тысячу лет назад за такой идиотизм :3
            std::tuple<size_t, size_t, double> bus_stats = TransportCatalogue::GetBusStats(bus);
            output << "Bus " << id << ": " <<
                std::get<0>(bus_stats)
            << " stops on route, "
            << std::get<1>(bus_stats)
            << " unique stops, "
            << FloatToString<double>(std::get<2>(bus_stats), 6)  << " route length\n";
        }
    } else if (type == "Stop") {
        const Stop *stop = transport_catalogue.FindStop(id);
        if (!stop) {
            output << "Stop " << id << ": not found\n";
        } else {
            if (auto buslist = transport_catalogue.FindStopBusList(stop->name); buslist == nullptr) {
                output << "Stop " << id << ": no buses\n";
            } else {
                ///>>зачем-то получаете список
                ///тупая яяяя, см выше
                output << "Stop " << id << ": buses " + BusListToString(buslist) + "\n";
            }
        }
    }
}

template<typename T, typename>
std::string Parcer::FloatToString(T num, const int precision){
    std::ostringstream oss;
    oss << std::setprecision(precision) << num;
    return oss.str();
}

std::string Parcer::BusListToString(const std::set<std::string>* buslist) {
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
