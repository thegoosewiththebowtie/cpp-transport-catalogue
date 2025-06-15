#pragma once
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

#include "geo.h"

///дико извиняюсь за такое отставание, у меня в жизни ка*пец был, но я нагоню честно-честно, я теперь безработная, буду 24/7 кодить хыхы

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
	void AddStop(std::string_view name, Coordinates coords);
	void AddBus(std::string_view name, const std::vector<std::string_view>& stop_names, bool is_roundtrip);

	const Stop* FindStop(std::string_view name) const;
	const Bus* FindBus(std::string_view name) const;

	size_t GetBusStopCount(std::string_view bus_name) const;
	size_t GetUniqueStopCount(std::string_view bus_name) const;

	const std::unordered_map<std::string, const Bus*>* GetBusList (std::string_view stop_name) const;

private:
	std::unordered_map<std::string, std::unordered_map<std::string, const Bus*>> buses_by_stop;
	std::unordered_map<std::string, Stop> stops_;
	std::unordered_map<std::string, Bus> buses_;
};
