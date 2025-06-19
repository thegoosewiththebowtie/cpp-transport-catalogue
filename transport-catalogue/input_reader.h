#pragma once
#include <string>
#include <string_view>
#include <vector>
#include "transport_catalogue.h"

struct CommandDescription {
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;      // Название команды
    std::string id;           // id маршрута или остановки
    std::string description;  // Параметры команды
};

class InputReader {
public:
    void ParseLine(std::string_view line);

    void ReadBaseInput(std::istream &ins, TransportCatalogue &transport_catalogue);

    void ReadStatInput(std::istream &ins, std::ostream &ous, const TransportCatalogue &transport_catalogue);

    void ApplyCommands(TransportCatalogue& catalogue) const;


private:
    std::vector<CommandDescription> commands_;
};
