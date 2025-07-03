/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#include "in_parser.h"

#include <memory>

#include "geo.h"
#include "transport_catalogue.h"
#include "jsonLib/jsonlib.h"
#include "salatLib/salatlib.h"
namespace transport_catalogue::parser {
    class OutParser;
    class InParser;

    void InParser::Deserialize(std::istream&                           arg_istream
                          , const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue
                          , const std::unique_ptr<sRequests>&          arg_common_request_data) {
        SALAT_SALTBEGIN();
        const std::unique_ptr<jsonlib::Document> doc = std::make_unique<jsonlib::Document>(jsonlib::gLoad(arg_istream));
        std::map<std::string , jsonlib::Node>    root = std::move(doc->GetRoot().AsMutable<jsonlib::dict_TD>());
        const jsonlib::Node&                     basenode = doc->GetRoot();
        switch(basenode.GetValueType()) {
            case jsonlib::DICT_TD : break;
            case jsonlib::ARRAY_TD :
            case jsonlib::NULLPTR_T :
            case jsonlib::BOOL :
            case jsonlib::INT :
            case jsonlib::DOUBLE :
            case jsonlib::STRING : SALAT_RINTHR("recieved, expected map");
        }
        arg_common_request_data->main_map = std::move(root);
        if(arg_common_request_data->main_map.size() > 3 || arg_common_request_data->main_map.size() < 2) {
            SALAT_RINTHR("recieved a map of " + std::to_string(arg_common_request_data.get()->main_map.size()) +
                         " elements, expected 2 or 3");
        }
        SALAT_SALTCTMFBEGIN("base_req");
        if(arg_common_request_data->main_map.find("base_requests") != arg_common_request_data->main_map.end() && !
            arg_common_request_data->main_map.at("base_requests").As<jsonlib::array_TD>().empty()) {
            arg_common_request_data->input_requests = &arg_common_request_data->main_map.at("base_requests").AsMutable<
                jsonlib::array_TD>();
        }
        else { arg_common_request_data->input_requests = {}; }
        SALAT_SALTCTMFEND("base_req");
        SALAT_SALTCTMFBEGIN("stat_req");
        if(arg_common_request_data->main_map.find("stat_requests") != arg_common_request_data->main_map.end() && !
            arg_common_request_data->main_map.at("stat_requests").As<jsonlib::array_TD>().empty()) {
            arg_common_request_data->output_requests = &arg_common_request_data->main_map.at("stat_requests").AsMutable<
                jsonlib::array_TD>();
        }
        else { arg_common_request_data->output_requests = {}; }
        SALAT_SALTCTMFEND("stat_req");
        SALAT_SALTCTMFBEGIN("render_settings");
        if(arg_common_request_data->main_map.find("render_settings") != arg_common_request_data->main_map.end() && !
            arg_common_request_data->main_map.at("render_settings").As<jsonlib::dict_TD>().empty()) {
            arg_common_request_data->render_settings = &arg_common_request_data->main_map.at("render_settings").
                                                                                 AsMutable<jsonlib::dict_TD>();
        }
        else { arg_common_request_data->render_settings = {}; }
        SALAT_SALTCTMFEND("render_settings");
        ProcessArrays(arg_transport_catalogue, arg_common_request_data);
        SALAT_SALTEND();
    }

    void InParser::ProcessArrays(const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue
                               , const std::unique_ptr<sRequests>&          arg_common_request_data) {
        SALAT_BEGIN();
        if(arg_common_request_data->input_requests == nullptr) {
            return;
        }
        SALAT_CTMFBEGIN("ProcessBus");
        SALAT_CTMFBEGIN("ProcessStop");
        for(const jsonlib::Node& element : *arg_common_request_data->input_requests) {
            const std::map<std::string , jsonlib::Node>* element_map = &element.As<jsonlib::dict_TD>();
            std::string                                  type        = element_map->at("type").As<std::string>();
            if(!(type == "Bus"
                     ? ProcessBus(element_map, arg_transport_catalogue)
                     : ProcessStop(element_map, arg_transport_catalogue))) {
                SALAT_RINTHR("BUS OR STOP PROCESSING FAILED");
            }
            SALAT_BREAK();
        }
        SALAT_CTMFEND("ProcessStop");
        SALAT_CTMFEND("ProcessBus");
        SALAT_END();
    }

    bool InParser::ProcessBus(const std::map<std::string , jsonlib::Node>* arg_busmap
                            , const std::unique_ptr<TransportCatalogue>&   arg_transport_catalogue) {
        ///BEGINNAME
        const auto name = arg_busmap->find("name");
        if(name == arg_busmap->end()) { return false; }
        const std::string& busname = name->second.As<std::string>();
        ///BEGINSTOPS                    NAMEEND///
        const auto stops = arg_busmap->find("stops");
        if(stops == arg_busmap->end()) { return false; }
        thread_local std::vector<std::string_view> busstops;
        busstops.clear();
        busstops.reserve(stops->second.As<jsonlib::array_TD>().size());
        for(const jsonlib::Node& stop : stops->second.As<jsonlib::array_TD>()) {
            busstops.push_back(stop.As<std::string>());
        }
        ///BEGINISROUNDTRIP            STOPSEND///
        const auto isroundtrip = arg_busmap->find("is_roundtrip");
        if(isroundtrip == arg_busmap->end()) { return false; }
        const bool busisroundtrip = isroundtrip->second.As<bool>();
        ///                     ISROUNDTRIPEND///
        arg_transport_catalogue->AddBus(busname, busstops, busisroundtrip);
        SALAT_BREAK();
        return true;
    }

    bool InParser::ProcessStop(const std::map<std::string , jsonlib::Node>* arg_stopmap
                             , const std::unique_ptr<TransportCatalogue>&   arg_transport_catalogue) {
        ///BEGINNAME
        const auto name = arg_stopmap->find("name");
        if(name == arg_stopmap->end()) { return false; }
        const std::string& stopname = name->second.As<std::string>();
        ///BEGINCOORDINATES         NAMEEND///
        const auto latitude = arg_stopmap->find("latitude");
        if(latitude == arg_stopmap->end()) { return false; }
        const double& stoplatitude = latitude->second.As<double>();

        const auto longitude = arg_stopmap->find("longitude");
        if(longitude == arg_stopmap->end()) { return false; }
        const double&               stoplongitude = longitude->second.As<double>();
        const geo::Geo::Coordinates stopcoordinates{stoplatitude , stoplongitude};
        ///BEGINROUTES       COORDINATESEND///
        const auto road_distances = arg_stopmap->find("road_distances");
        if(road_distances != arg_stopmap->end()) {
            for(const auto& [stopname2, distance] : road_distances->second.As<jsonlib::dict_TD>()) {
                arg_transport_catalogue->AddStopsRoute(stopname, stopname2, distance.As<int>());
            }
        }
        ///                       ROUTESEND///
        arg_transport_catalogue->AddStop(stopname, stopcoordinates);
        SALAT_BREAK();
        return true;
    }
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
