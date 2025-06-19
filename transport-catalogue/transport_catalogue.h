#pragma once
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "geo.h"
struct Bus;

struct Stop {
	std::string name;
	Coordinates coordinates;
};
struct Bus {
	std::string name;
	std::vector<const Stop*> stops;
	bool is_roundtrip;
};

class TransportCatalogue {
public:
	void AddStop(std::string_view name, const Coordinates& coords);
	void AddBus(std::string_view name, const std::vector<std::string_view>& stop_names, bool is_roundtrip);

	const Stop* FindStop(std::string_view name) const;
	const Bus* FindBus(std::string_view name) const;
	const std::set<std::string> *FindStopBusList(std::string_view name) const;

	static std::tuple<size_t, size_t, double> GetBusStats(const Bus *bus);

private:
	static size_t GetBusStopCount(const Bus *bus) ;
	static size_t GetUniqueStopCount(const Bus *bus);
	static double GetRouteGeoDistance(const Bus *bus);

	std::unordered_map<std::string, std::set<std::string>> buses_by_stop_;
	std::unordered_map<std::string, Stop> stops_;
	std::unordered_map<std::string, Bus> buses_;
};
