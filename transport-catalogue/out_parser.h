/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <iosfwd>
#include <memory>
#include <vector>

#include "jsonLib/jsonlib.h"
namespace transport_catalogue {
    class TransportCatalogue;
}
namespace transport_catalogue::parser {
    class OutParser {
        public:
            static void Serialize(std::ostream&                              arg_output
                                , const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue
                                , const std::vector<jsonlib::Node>*          arg_output_requests
                                , const std::stringstream&                   arg_svgstream);
        private:
            static jsonlib::Node BusToNode(int                                        arg_id
                                         , const std::string&                         arg_bus_name
                                         , const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue);
            static jsonlib::Node StopToNode(int                                        arg_id
                                          , const std::string&                         arg_stop_name
                                          , const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue);
            static jsonlib::Node SvgToNode(int arg_id , const std::stringstream& arg_svgstream);
    };
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
