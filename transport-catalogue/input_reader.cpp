#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <istream>
#include <iterator>

#include "stat_reader.h"

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
        Parser::ParseAndPrintStat(transport_catalogue, line, ous);
    }
}

void InputReader::ParseLine(const std::string_view line) {
    if (CommandDescription cmd = ParseCommandDescription(line)) {
        commands_.push_back(std::move(cmd));
    }
}

void InputReader::ApplyCommands(TransportCatalogue& catalogue) const {
    for (const auto &[command, id, description]: commands_) {
        if (command == "Stop") {
            Geo::Coordinates coords = ParseCoordinates(description);
            catalogue.AddStop(id, coords);
            ParseAndSetStopDistances(id, description, catalogue);
        } else if (command == "Bus") {
            const bool is_roundtrip = (description.find('>') != std::string::npos);
            std::vector<std::string_view> stops = ParseRoute(description);
            catalogue.AddBus(id, stops, is_roundtrip);
        }
    }
}

Geo::Coordinates InputReader::ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");
    const std::size_t not_space = str.find_first_not_of(' ');
    const std::size_t comma = str.find(',');
    if (comma == std::string_view::npos) {
        return {nan, nan};
    }
    const auto not_space2 = str.find_first_not_of(' ', comma + 1);
    const double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    const double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

std::string_view InputReader::Trim(std::string_view string) {
    const std::size_t start = string.find_first_not_of(' ');
    if (start == std::string_view::npos) { return {}; }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

std::vector<std::string_view> InputReader::Split(std::string_view string, const char delim) {
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

std::vector<std::string_view> InputReader::ParseRoute(std::string_view route) {
    if (route.find('>') != std::string_view::npos) { return Split(route, '>'); }
    std::vector<std::string_view> stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());
    return results;
}

CommandDescription InputReader::ParseCommandDescription(std::string_view line) {
    const std::size_t colon_pos = line.find(':');
    if (colon_pos == std::string_view::npos) { return {}; }
    const std::size_t space_pos = line.find(' ');
    if (space_pos >= colon_pos) { return {}; }
    const std::size_t not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) { return {}; }
    return {  std::string(line.substr(0, space_pos)),
                std::string(line.substr(not_space, colon_pos - not_space)),
                std::string(line.substr(colon_pos + 1))};
}

void InputReader::ParseAndSetStopDistances(const std::string &stop_name, std::string_view description, TransportCatalogue &catalogue) {
    const std::size_t comma_pos = description.find(',');
    if (comma_pos == std::string_view::npos) { return; }
    const std::size_t second_comma = description.find(',', comma_pos + 1);
    if (second_comma == std::string_view::npos) { return; }
    const std::string_view rest = description.substr(second_comma + 1);
    std::vector<std::string_view> parts = Split(rest, ',');
    for (std::string_view part : parts) {
        part = Trim(part);
        const std::size_t m_pos = part.find("m to ");
        if (m_pos == std::string_view::npos) { continue; }
        const int distance = std::stoi(std::string(part.substr(0, m_pos)));
        const std::string_view to_stop = Trim(part.substr(m_pos + 5));
        catalogue.AddStopsRoute(stop_name, to_stop, distance);
    }
}
