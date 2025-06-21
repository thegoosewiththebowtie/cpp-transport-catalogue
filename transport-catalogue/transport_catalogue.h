#pragma once
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "geo.h"
struct BusStats;
class TransportCatalogue;
struct Stop {
	std::string name;
	Geo::Coordinates coordinates;
};
struct Bus {
	std::string name;
	std::vector<Stop*> stops;
	bool is_roundtrip;
};
class TransportCatalogue {
public:
	void AddStop(std::string_view name, const Geo::Coordinates &coords);
	void AddBus(std::string_view name, const std::vector<std::string_view>& stop_names, bool is_roundtrip);
	void AddStopsRoute(std::string_view name1, std::string_view name2, int distance);

	const Stop* FindStop(std::string_view name) const;
	const Bus* FindBus(std::string_view name) const;
	const std::set<std::string> *FindStopBusList(std::string_view name) const;

	BusStats GetBusStats(const Bus *bus) const;
private:
	static size_t GetBusStopCount(const Bus *bus) ;
	static size_t GetUniqueStopCount(const Bus *bus);
	static double GetRouteGeoDistance(const Bus *bus);

	int GetStopsDistance(Stop *stop1, Stop *stop2) const;
	int GetRouteFactualDistance(const Bus *bus) const;

	friend BusStats;

	std::unordered_map<std::string, std::set<std::string>> buses_by_stop_;
	std::unordered_map<std::string, Stop> stops_;
	std::unordered_map<std::string, Bus> buses_;
	struct RDHasher {
		size_t operator()(const std::pair<Stop*, Stop*>& p) const {
			const size_t h1 = std::hash<Stop*>{}(p.first);
			const size_t h2 = std::hash<Stop*>{}(p.second);
			return h1 ^ (h2 << 1);
		}
	};
	std::unordered_map<std::pair<Stop*, Stop*>, int, RDHasher> road_distance_;
};
struct BusStats {
	BusStats(const Bus* bus, const TransportCatalogue& transport_catalogue)
	: stop_count(TransportCatalogue::GetBusStopCount(bus))
	, unique_stop_count(TransportCatalogue::GetUniqueStopCount(bus))
	, factual_distance(transport_catalogue.GetRouteFactualDistance(bus))
	, curvature(Geo::GetRouteCurvature(factual_distance, TransportCatalogue::GetRouteGeoDistance(bus))){}
	size_t stop_count;
	size_t unique_stop_count;
	int factual_distance;
	double curvature;
};
