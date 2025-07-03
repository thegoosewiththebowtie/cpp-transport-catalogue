/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <iomanip>
#include <iosfwd>
#include <map>
#include <memory>
#include <stack>
#include <vector>
namespace jsonlib {
    class Node;
}
namespace transport_catalogue {
    class TransportCatalogue;
    namespace parser {
    struct sRequests {
        std::map<std::string , jsonlib::Node>  main_map;
        std::vector<jsonlib::Node>*            input_requests{};
        std::map<std::string , jsonlib::Node>* render_settings{};
        std::vector<jsonlib::Node>*            output_requests{};
    };
    class InParser {
        public:
            static void Deserialize(std::istream&                              arg_istream
                                  , const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue
                                  , const std::unique_ptr<sRequests>&          arg_common_request_data);
            static void ProcessArrays(const std::unique_ptr<TransportCatalogue>& arg_transport_catalogue
                                    , const std::unique_ptr<sRequests>&          arg_common_request_data);
            static bool ProcessBus(const std::map<std::string , jsonlib::Node>* arg_busmap
                                 , const std::unique_ptr<TransportCatalogue>&   arg_transport_catalogue);
            static bool ProcessStop(const std::map<std::string , jsonlib::Node>* arg_stopmap
                                  , const std::unique_ptr<TransportCatalogue>&   arg_transport_catalogue);
    };
}}

/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
