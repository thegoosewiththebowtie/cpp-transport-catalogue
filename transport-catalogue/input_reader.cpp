#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <istream>
#include <iterator>

#include "stat_reader.h"

Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    std::size_t not_space = str.find_first_not_of(' ');
    std::size_t comma = str.find(',');

    if (comma == std::string_view::npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

std::string_view Trim(std::string_view string) {
    const std::size_t start = string.find_first_not_of(' ');
    if (start == std::string_view::npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        std::size_t delim_pos = string.find(delim, pos);
        if (delim_pos == std::string_view::npos) {
            delim_pos = string.size();
        }
        if (std::string_view substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }
    return result;
}

std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != std::string_view::npos) {
        return Split(route, '>');
    }
    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());
    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    std::size_t colon_pos = line.find(':');
    if (colon_pos == std::string_view::npos) { return {}; }
    std::size_t space_pos = line.find(' ');
    if (space_pos >= colon_pos) { return {}; }
    std::size_t not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) { return {}; }
    return {  std::string(line.substr(0, space_pos)),
                std::string(line.substr(not_space, colon_pos - not_space)),
                std::string(line.substr(colon_pos + 1))};
}

void InputReader::ReadBaseInput(std::istream& ins, TransportCatalogue& transport_catalogue) {
    int base_request_count;
    ins >> base_request_count >> std::ws;
    {
        for (int i = 0; i < base_request_count; ++i) {
            std::string line;
            getline(ins, line);
            ParseLine(line);
        }
        ApplyCommands(transport_catalogue);
    }
}

void InputReader::ReadStatInput(std::istream& ins, std::ostream& ous, const TransportCatalogue& transport_catalogue) {
    int stat_request_count;
    ins >> stat_request_count >> std::ws;
    for (int i = 0; i < stat_request_count; ++i) {
        std::string line;
        getline(ins, line);
        ParseAndPrintStat(transport_catalogue, line, ous);
    }
}

void InputReader::ParseLine(const std::string_view line) {
    if (CommandDescription command_description = ParseCommandDescription(line)) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands(TransportCatalogue& catalogue) const {
    for (const CommandDescription &cmd: commands_) {
        if (cmd.command == "Stop") {
            Coordinates coords = ParseCoordinates(cmd.description);
            catalogue.AddStop(cmd.id, coords);
        } else if (cmd.command == "Bus") {
            bool is_roundtrip = cmd.description.contains('>');
            std::vector<std::string_view> stops = ParseRoute(cmd.description);
            catalogue.AddBus(cmd.id, stops, is_roundtrip);
        }
    }
}
