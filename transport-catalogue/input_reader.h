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

    std::string command;
    std::string id;
    std::string description;
};

class InputReader {
public:
    void ParseLine(std::string_view line);
    void ReadBaseInput(std::istream &ins, TransportCatalogue &transport_catalogue);
    void ApplyCommands(TransportCatalogue& catalogue) const;

    static void ReadStatInput(std::istream &ins, std::ostream &ous, const TransportCatalogue &transport_catalogue);

private:
    static Geo::Coordinates ParseCoordinates(std::string_view str);
    static std::string_view Trim(std::string_view string);
    static std::vector<std::string_view> Split(std::string_view string, char delim);
    static std::vector<std::string_view> ParseRoute(std::string_view route);
    static CommandDescription ParseCommandDescription(std::string_view line);
    static void ParseAndSetStopDistances(const std::string &stop_name, std::string_view description, TransportCatalogue &catalogue);

    std::vector<CommandDescription> commands_;
};
