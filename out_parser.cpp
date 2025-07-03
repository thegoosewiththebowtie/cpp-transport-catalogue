/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#include "out_parser.h"

#include "common.h"
#include "transport_catalogue.h"
#include "salatLib/salatlib.h"
namespace transport_catalogue::parser {
    void OutParser::Serialize(std::ostream&                              arg_output
                            , const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue
                            , const std::vector<jsonlib::Node>*          arg_output_requests
                            , const std::stringstream&                   arg_svgstream) {
        SALAT_SALTBEGIN();
        SALAT_CTMFBEGIN("BusToNode");
        SALAT_CTMFBEGIN("StopToNode");
        jsonlib::Node doctoprint = std::vector<jsonlib::Node>{};

        for(const jsonlib::Node& request : *arg_output_requests) {
            const std::map<std::string , jsonlib::Node>& current_request = request.As<jsonlib::dict_TD>();
            auto                                         id              = current_request.find("id");
            if(id == current_request.end()) { SALAT_RINTHR("NO ID IN REQUEST"); }
            auto type = current_request.find("type");
            if(type == current_request.end()) { SALAT_RINTHR("NO TYPE IN REQUEST"); }
            auto name = current_request.find("name");
            if(name == current_request.end() && type->second.As<std::string>() != "Map") {
                SALAT_RINTHR("NO NAME IN REQUEST");
            }

            jsonlib::Node toprint;
            switch(type->second.As<std::string>()[0]) {
                case 'B' : toprint = BusToNode(id->second.As<int>()
                                             , name->second.As<std::string>()
                                             , arg_transport_catalogue);
                    break;
                case 'S' : toprint = StopToNode(id->second.As<int>()
                                              , name->second.As<std::string>()
                                              , arg_transport_catalogue);
                    break;
                case 'M' : toprint = SvgToNode(id->second.As<int>(), arg_svgstream);
                    break;
                default : SALAT_RINTHR("INCORRECT TYPE IN REQUEST");
            }
            doctoprint.AsMutable<jsonlib::array_TD>().push_back(std::move(toprint));
        }
        SALAT_CTMFEND("BusToNode");
        SALAT_CTMFEND("StopToNode");
        jsonlib::gPrint(doctoprint, arg_output);
        SALAT_SALTEND();
    }

    jsonlib::Node OutParser::BusToNode(int                                        arg_id
                                     , const std::string&                         arg_bus_name
                                     , const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue) {
        std::map<std::string , jsonlib::Node> retnode;
        const common::sBus*                   bus = arg_transport_catalogue->FindBus(arg_bus_name);
        retnode["request_id"]                     = jsonlib::Node{arg_id};
        if(bus == nullptr) {
            retnode["error_message"] = "not found";
            SALAT_BREAK();
            return retnode;
        }
        auto [stop_count, unique_stop_count, factual_distance, curvature] = arg_transport_catalogue->GetBusStats(bus);
        retnode["curvature"] = jsonlib::Node{curvature};
        retnode["route_length"] = jsonlib::Node{factual_distance};
        retnode["stop_count"] = jsonlib::Node{static_cast<int>(stop_count)};
        retnode["unique_stop_count"] = jsonlib::Node{static_cast<int>(unique_stop_count)};
        SALAT_BREAK();
        return retnode;
    }

    jsonlib::Node OutParser::StopToNode(int                                        arg_id
                                      , const std::string&                         arg_stop_name
                                      , const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue) {
        std::map<std::string , jsonlib::Node> retnode;
        retnode["request_id"]     = jsonlib::Node{arg_id};
        const common::sStop* stop = arg_transport_catalogue->FindStop(arg_stop_name);
        if(stop == nullptr) {
            retnode["error_message"] = "not found";
            SALAT_BREAK();
            return retnode;
        }
        const std::set<std::string>* buses = arg_transport_catalogue->FindStopBusList(arg_stop_name);
        jsonlib::array_TD            busesvector;
        if(buses == nullptr) { busesvector = std::vector<jsonlib::Node>{}; }
        else { for(const std::string& busname : *buses) { busesvector.emplace_back(std::string(busname)); } }
        retnode["buses"] = jsonlib::Node{std::move(busesvector)};
        SALAT_BREAK();
        return retnode;
    }

    jsonlib::Node OutParser::SvgToNode(int arg_id , const std::stringstream& arg_svgstream) {
        std::map<std::string , jsonlib::Node> retnode;
        retnode["request_id"] = jsonlib::Node{arg_id};
        retnode["map"]        = arg_svgstream.str();
        return retnode;
    }
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
