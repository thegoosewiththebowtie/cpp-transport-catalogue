/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <set>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "domain.h"
#include "geo.h"

namespace transport_catalogue {
    /*[BEGIN:========================================TRANSPORTCATALOGUE==============================================]*/
    class TransportCatalogue {
        public:
            //добавляет остановку
            void AddStop(std::string_view arg_name , const common::Coordinates& arg_coords);
            //добавляет автобус, создает при необходимости новую пустую и передает указатель в автобус
            void AddBus(std::string_view                     arg_name
                      , const std::vector<std::string_view>& arg_stop_names
                      , bool                                 arg_is_roundtrip);

            void AddStopsRoute(std::string_view arg_name1 , std::string_view arg_name2 , double arg_distance);

            const common::sStop*         FindStop(std::string_view arg_name) const;
            const common::sBus*          FindBus(std::string_view arg_name) const;
            const std::set<std::string>* FindStopBusList(std::string_view arg_name) const;

            common::sBusStats                                      GetBusStats(const common::sBus* arg_bus) const;
            std::vector<std::string_view>                          GetSortedBusesView(bool arg_onlywithstops) const;
            std::vector<const common::sBus*>                       GetSortedBuses(bool arg_onlywithstops) const;
            std::vector<std::string_view>                          GetSortedStopsView(bool arg_onlywithbuses) const;
            std::vector<const common::sStop*> GetSortedStops(bool arg_onlywithbuses) const;
            const std::unordered_map<std::string , common::sBus>*  GetUnsortedBuses() const;
            const std::unordered_map<std::string , common::sStop>* GetUnsortedStops() const;
        private:
            static size_t GetBusStopCount(const common::sBus* arg_bus);
            static size_t GetUniqueStopCount(const common::sBus* arg_bus);
            static double GetRouteGeoDistance(const common::sBus* arg_bus);

            double GetStopsDistance(common::sStop* arg_stop1 , common::sStop* arg_stop2) const;
            double GetRouteFactualDistance(const common::sBus* arg_bus) const;

            std::unordered_map<std::string , std::set<std::string>> buses_by_stop_;
            std::unordered_map<std::string , common::sStop>         stops_;
            std::unordered_map<std::string , common::sBus>          buses_;
            struct RoadDistanceHasher {
                RoadDistanceHasher(const RoadDistanceHasher&) = default;
                RoadDistanceHasher() = default;
                size_t operator()(const std::pair<common::sStop* , common::sStop*>& arg_p) const {
                    const size_t h1 = std::hash<common::sStop*>{}(arg_p.first);
                    const size_t h2 = std::hash<common::sStop*>{}(arg_p.second);
                    return h1 ^ (h2 << 1);
                }
            };
            std::unordered_map<std::pair<common::sStop* , common::sStop*> , double , RoadDistanceHasher> road_distance_;
    };
    /*[END:=========================================TRANSPORTCATALOGUE==============================================]*/
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
