/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#include <fstream>

#include "in_parser.h"
#include "maprenderer.h"
#include "out_parser.h"
#include "transport_catalogue.h"
#include "salatLib/salatlib.h"
bool gRender_only    = false;
class Processing {
    public:
        static void ProcessData(std::istream&                         arg_from
                              , std::ostream& arg_to
                              , const std::unique_ptr<transport_catalogue::TransportCatalogue>&
                                arg_transport_catalogue) {
            SALAT_SALTBEGIN();
            arg_to << std::setprecision(6);
            const std::unique_ptr<transport_catalogue::parser::sRequests> common_request_data = std::make_unique<
                transport_catalogue::parser::sRequests>();
            transport_catalogue::parser::InParser::Deserialize(arg_from, arg_transport_catalogue, common_request_data);
            transport_catalogue::map_renderer::MapRenderer map_renderer(arg_transport_catalogue
                                                                      , *common_request_data->render_settings);
            if(gRender_only) {
                map_renderer.Render(arg_to);
            } else {
                std::stringstream ss;
                ss << std::setprecision(6);
                map_renderer.Render(ss);
                transport_catalogue::parser::OutParser::Serialize(arg_to
                                                                , arg_transport_catalogue
                                                                , common_request_data->output_requests
                                                                , ss);
            }

            SALAT_SALTEND();
        }
};

int main(int arg_c , char* arg_v[]) {
    SALAT_SETOUTPUT(std::cout);

    SALAT_SETOUTPUT(std::cout);
    for(int i = 1 ; i < arg_c ; ++i) { if(std::string(arg_v[i]) == "--render-only") { gRender_only = true; } }
    std::ifstream in("Q:/Projects/TransportStuff/a.in.txt");
    std::ofstream out("Q:/Projects/TransportStuff/a.output.txt"); {
        std::unique_ptr<transport_catalogue::TransportCatalogue> transport_catalogue_var = std::make_unique<
            transport_catalogue::TransportCatalogue>();
        Processing::ProcessData(in, out, transport_catalogue_var);
    }
    in.close();
    out.close();
    return SALAT_FULLFICO("Q:/Projects/TransportStuff/a.output.txt", "Q:/Projects/TransportStuff/a.out.txt");
}

/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
