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
    //нуштош. комменты так комменты.
    SALAT_SETOUTPUT(std::cout);
    for(int i = 1 ; i < arg_c ; ++i) {
        //обрабатываем флаги (первый был в тестах, второй - мое предположение)
        if(std::string(arg_v[i]) == "--render-only") {
            gOutput_filter = transport_catalogue::common::eOutputFilter::MAP;
        }
        if(std::string(arg_v[i]) == "--stats-only") {
            gOutput_filter = transport_catalogue::common::eOutputFilter::REQUESTS;
        }
    }
    std::ifstream in("F:/Libraries/Projects/b.TransportCatalogue-v2/a.in.txt");
    std::ofstream out("F:/Libraries/Projects/b.TransportCatalogue-v2/a.output.txt");
    if(in.fail()|| out.fail()) {
        std::cerr << "fuck";
    }
    out << std::setprecision(6);
    {
        //создаем transport_catalogue в куче тк он может хранить довольно много данных
        std::unique_ptr<transport_catalogue::TransportCatalogue> transport_catalogue_var = std::make_unique<
            transport_catalogue::TransportCatalogue>();
        Processing::ProcessData(in, out, transport_catalogue_var);
    }
    out.flush();
    in.close();
    out.close();
    return SALAT_FULLFICO("F:/Libraries/Projects/b.TransportCatalogue-v2/a.output.txt"
                        , "F:/Libraries/Projects/b.TransportCatalogue-v2/a.out.txt"
                        , true
                        , true);
}

/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
