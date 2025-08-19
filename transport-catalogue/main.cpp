/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#include <fstream>

#include "parser.h"
#include "transport_catalogue.h"
#include "salatLib/salatlib.h"
transport_catalogue::common::eOutputFilter gOutput_filter = transport_catalogue::common::eOutputFilter::NONE;
class Processing {
    public:
        static void ProcessData(std::istream& arg_from
                              , std::ostream& arg_to
                              , const std::unique_ptr<transport_catalogue::TransportCatalogue>&
                                arg_transport_catalogue) {
            transport_catalogue::parser::Parser parser(arg_transport_catalogue.get());
            parser.Deserialize(arg_from);
            parser.Serialize(arg_to, gOutput_filter);
        }
};

int main(int arg_c , char* arg_v[]) {
    for(int i = 1 ; i < arg_c ; ++i) {
        if(std::string(arg_v[i]) == "--render-only") {
            gOutput_filter = transport_catalogue::common::eOutputFilter::MAP;
        }
        if(std::string(arg_v[i]) == "--stats-only") {
            gOutput_filter = transport_catalogue::common::eOutputFilter::REQUESTS;
        }
    }
    std::ostream& out = std::cout;
    std::istream& in = std::cin;
    if(in.fail()|| out.fail()) {
        std::cerr << "fuck";
    }
    out << std::setprecision(6);
    {
        std::unique_ptr<transport_catalogue::TransportCatalogue> transport_catalogue_var = std::make_unique<
            transport_catalogue::TransportCatalogue>();
        Processing::ProcessData(in, out, transport_catalogue_var);
    }
    out.flush();
}

/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
