/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#include "parser.h"

#include <memory>
#include <ranges>

#include "geo.h"
#include "transport_catalogue.h"
#include "jsonLib/jsonlib.h"
#include "salatLib/salatlib.h"
namespace transport_catalogue::parser {
    void Parser::ChangeCatalogue(TransportCatalogue* arg_transport_catalogue) {
        transport_catalogue_ = arg_transport_catalogue;
        map_data_            = {};
        render_settings_     = {};
        requests_            = {};
    }

    void Parser::Deserialize(std::istream& arg_istream) {
        SALAT_SALTBEGIN();
        const std::unique_ptr<jsonlib::Document> doc = std::make_unique<jsonlib::Document>(jsonlib::gLoad(arg_istream));
        std::map<std::string , jsonlib::Node>    root = std::move(doc->GetRoot().AsMutable<jsonlib::dict_TD>());
        const jsonlib::Node&                     basenode = doc->GetRoot();
        if(basenode.GetValueType() != jsonlib::DICT_TD) { SALAT_RINTHR("wrong type recieved, expected map"); }
        requests_.main_map = std::move(root);
        if(requests_.main_map.size() != 3) {
            SALAT_RINTHR("recieved a map of " + std::to_string(requests_.main_map.size()) +
                         " elements, expected 2 or 3");
        }
        if(requests_.main_map.find("base_requests") != requests_.main_map.end() && !requests_.main_map.
                                                                                              at("base_requests").As<
                                                                                                  jsonlib::array_TD>().
                                                                                              empty()) {
            requests_.input_requests = &requests_.main_map.at("base_requests").AsMutable<jsonlib::array_TD>();
        }
        else { requests_.input_requests = {}; }
        if(requests_.main_map.find("stat_requests") != requests_.main_map.end() && !requests_.main_map.
                                                                                              at("stat_requests").As<
                                                                                                  jsonlib::array_TD>().
                                                                                              empty()) {
            requests_.output_requests = &requests_.main_map.at("stat_requests").AsMutable<jsonlib::array_TD>();
        }
        else { requests_.output_requests = {}; }
        if(requests_.main_map.find("render_settings") != requests_.main_map.end() && !requests_.main_map.
                                                                                                at("render_settings").As
                                                                                                <jsonlib::dict_TD>().
                                                                                                empty()) {
            SetRenderSettingsFromNode(requests_.main_map.at("render_settings").AsMutable<jsonlib::dict_TD>());
        }
        else { requests_.render_settings = {}; }
        for(const jsonlib::Node& element : *requests_.input_requests) {
            const std::map<std::string , jsonlib::Node> element_map = element.As<jsonlib::dict_TD>();
            std::string                                 type        = element_map.at("type").As<std::string>();
            switch(type[0]) {
                case 'B' : AddNodeBus(element_map);
                    break;
                case 'S' : AddNodeStop(element_map);
                    break;
                default : SALAT_RINTHR("INCORRECT TYPE IN REQUEST");
            }
        }
        map_data_ = geo::Geo::CalculateMapData(transport_catalogue_->GetSortedStops(true), render_settings_);
        SALAT_SALTEND();
    }

    void Parser::Serialize(std::ostream&         arg_output
                         , common::eOutputFilter arg_output_filter = common::eOutputFilter::NONE) {
        SALAT_SALTBEGIN();
        //в тестах был параметр --render_only, поэтому сразу реализую и render_only и requests_only
        bool printmap   = false;
        bool printstats = false;
        switch(arg_output_filter) {
            case common::eOutputFilter::NONE : printmap = true;
                printstats = true;
                break;
            case common::eOutputFilter::MAP : printmap = true;
                break;
            case common::eOutputFilter::REQUESTS : printstats = true;
                break;
        }
        jsonlib::Node doctoprint = std::vector<jsonlib::Node>{};
        if(requests_.main_map.empty()) { SALAT_RINTHR("NO REQUESTS. SERIALIZE FIRST"); }
        for(const jsonlib::Node& request : *requests_.output_requests) {
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
                case 'B' : if(!printstats) { continue; }
                    toprint = BusToNode(id->second.As<int>(), name->second.As<std::string>());
                    break;
                case 'S' : if(!printstats) { continue; }
                    toprint = StopToNode(id->second.As<int>(), name->second.As<std::string>());
                    break;
                case 'M' : if(!printmap) { continue; }
                    toprint = GetSvgNode(id->second.As<int>());
                    break;
                default : SALAT_RINTHR("INCORRECT TYPE IN REQUEST");
            }
            doctoprint.AsMutable<jsonlib::array_TD>().push_back(std::move(toprint));
        }
        jsonlib::gPrint(doctoprint, arg_output);
        SALAT_SALTEND();
    }

    void Parser::AddNodeBus(const std::map<std::string , jsonlib::Node>& arg_busmap) const {
        //достает данные и кидает если их нет
        const auto name = arg_busmap.find("name");
        if(name == arg_busmap.end()) { SALAT_RINTHR("NO NAME IN REQUEST"); }
        const std::string& busname = name->second.As<std::string>();
        //фан факт - чатгепете не любит писать ошибки БОЛЬШИМИ БУКВАМИ И КОРОТКО, а напишет stops not found in stopmap
        //(да, я только что попросила его написать ошибку для этого примера, так что проверено)
        const auto stops = arg_busmap.find("stops");
        if(stops == arg_busmap.end()) { SALAT_RINTHR("NO STOPS IN REQUEST"); }
        /*thread_local std::vector<std::string_view> busstops; юзала thread_local чтобы избежать необходимости каждый
         *раз аллоцировать целый вектор, но раз уж упрощаем...*/
        std::vector<std::string_view> busstops;
        busstops.reserve(stops->second.As<jsonlib::array_TD>().size());
        for(const jsonlib::Node& stop : stops->second.As<jsonlib::array_TD>()) {
            busstops.push_back(stop.As<std::string>());
        }
        const auto isroundtrip = arg_busmap.find("is_roundtrip");
        if(isroundtrip == arg_busmap.end()) { SALAT_RINTHR("NO ROUNDRIP DATA IN REQUEST"); }
        const bool busisroundtrip = isroundtrip->second.As<bool>();
        transport_catalogue_->AddBus(busname, busstops, busisroundtrip);
    }

    void Parser::AddNodeStop(const std::map<std::string , jsonlib::Node>& arg_stopmap) const {
        //достает данные и кидает если их нет
        const auto name = arg_stopmap.find("name");
        if(name == arg_stopmap.end()) { SALAT_RINTHR("NO NAME IN REQUEST"); }
        const std::string& stopname = name->second.As<std::string>();
        const auto         latitude = arg_stopmap.find("latitude");
        if(latitude == arg_stopmap.end()) { SALAT_RINTHR("NO LATITUDE IN REQUEST"); }
        const double& stoplatitude = latitude->second.As<double>();
        const auto    longitude    = arg_stopmap.find("longitude");
        //ну и еще он не юзал бы мою библиотеку и написал throw std::runtime_error
        if(longitude == arg_stopmap.end()) { SALAT_RINTHR("NO LONGITUDE IN REQUEST"); }
        const double&               stoplongitude = longitude->second.As<double>();
        const common::Coordinates stopcoordinates{stoplatitude , stoplongitude};
        const auto                  road_distances = arg_stopmap.find("road_distances");
        if(road_distances != arg_stopmap.end() && !road_distances->second.As<jsonlib::dict_TD>().empty()) {
            for(const auto& [stopname2, distance] : road_distances->second.As<jsonlib::dict_TD>()) {
                transport_catalogue_->AddStopsRoute(stopname, stopname2, distance.As<double>());
            }
        }
        transport_catalogue_->AddStop(stopname, stopcoordinates);
    }

    void Parser::SetRenderSettingsFromNode(const std::map<std::string , jsonlib::Node>& arg_render_settings) {
        //убрала сейфгарды для большей простоты и, ну оно же не должно подавать пустые значения, да?
        render_settings_.width                = arg_render_settings.at("width").As<double>();
        render_settings_.height               = arg_render_settings.at("height").As<double>();
        render_settings_.padding              = arg_render_settings.at("padding").As<double>();
        render_settings_.line_width           = arg_render_settings.at("line_width").As<double>();
        render_settings_.stop_radius          = arg_render_settings.at("stop_radius").As<double>();
        render_settings_.bus_label_font_size  = arg_render_settings.at("bus_label_font_size").As<int>();
        render_settings_.bus_label_offset     = NodeToPoint(arg_render_settings.at("bus_label_offset"));
        render_settings_.stop_label_font_size = arg_render_settings.at("stop_label_font_size").As<int>();
        render_settings_.stop_label_offset    = NodeToPoint(arg_render_settings.at("stop_label_offset"));
        render_settings_.underlayer_color     = NodeToColor(arg_render_settings.at("underlayer_color"));
        render_settings_.underlayer_width     = arg_render_settings.at("underlayer_width").As<double>();
        //ну а тут оно всю color_palette закидывает в render_settings, idk сколько комментов писать tbh но пусть будет
        //не добавляет сразу в color_palette_ потому что color_palette_  - приватный, чтобы нужно было брать только
        //через GetNextColor чтобы продвинуться или GetColor если нужен предыдущий
        for(const jsonlib::Node& node : arg_render_settings.at("color_palette").As<jsonlib::array_TD>()) {
            render_settings_.AddColor(NodeToColor(node));
        }
    }

    svglib::color_TD Parser::NodeToColor(const jsonlib::Node& arg_node) {
        if(arg_node.GetValueType() != jsonlib::ARRAY_TD) {
            //возвращает строку если строка, кидает если не строка
            if(arg_node.GetValueType() == jsonlib::STRING) { return arg_node.As<std::string>(); }
            SALAT_RINCTMTTHR(std::invalid_argument, "INVALID COLOR TYPE");
        }
        //преобразует в sSolidColor если нет прозрачности и в прозрачный если она есть, кидает если array неправильный
        const std::vector<jsonlib::Node>& node = arg_node.As<jsonlib::array_TD>();
        switch(node.size()) {
            case 3 : return svglib::sSolidColor{static_cast<uint8_t>(node.at(0).As<int>())
                                              , static_cast<uint8_t>(node.at(1).As<int>())
                                              , static_cast<uint8_t>(node.at(2).As<int>())};
            case 4 : return svglib::sTransparentColor{static_cast<uint8_t>(node.at(0).As<int>())
                                                    , static_cast<uint8_t>(node.at(1).As<int>())
                                                    , static_cast<uint8_t>(node.at(2).As<int>())
                                                    , static_cast<float>(node.at(3).As<double>())};
            default : SALAT_RINCTMTTHR(std::invalid_argument, "INVALID ARRAY SIZE");
        }
    }

    svglib::sPoint Parser::NodeToPoint(const jsonlib::Node& arg_node) {
        const std::vector<jsonlib::Node>& node = arg_node.As<jsonlib::array_TD>();
        if(node.size() != 2) {
            //кидает если array не точка
            SALAT_RINCTMTTHR(std::invalid_argument, "INVALID ARRAY SIZE");
        }
        return {node.at(0).As<double>() , node.at(1).As<double>()};
    }

    jsonlib::Node Parser::BusToNode(int arg_id , const std::string& arg_bus_name) const {
        std::map<std::string , jsonlib::Node> retnode;
        const common::sBus*                   bus = transport_catalogue_->FindBus(arg_bus_name);
        retnode["request_id"]                     = jsonlib::Node{arg_id};
        if(bus == nullptr) {
            retnode["error_message"] = "not found";
            return retnode;
        }
        auto [stop_count, unique_stop_count, factual_distance, curvature] = transport_catalogue_->GetBusStats(bus);
        retnode["curvature"] = jsonlib::Node{curvature};
        retnode["route_length"] = jsonlib::Node{factual_distance};
        retnode["stop_count"] = jsonlib::Node{static_cast<int>(stop_count)};
        retnode["unique_stop_count"] = jsonlib::Node{static_cast<int>(unique_stop_count)};
        return retnode;
    }

    jsonlib::Node Parser::StopToNode(int arg_id , const std::string& arg_stop_name) const {
        std::map<std::string , jsonlib::Node> retnode;
        retnode["request_id"]     = jsonlib::Node{arg_id};
        const common::sStop* stop = transport_catalogue_->FindStop(arg_stop_name);
        if(stop == nullptr) {
            retnode["error_message"] = "not found";
            return retnode;
        }
        const std::set<std::string>* buses = transport_catalogue_->FindStopBusList(arg_stop_name);
        jsonlib::array_TD            busesvector;
        if(buses == nullptr) { busesvector = std::vector<jsonlib::Node>{}; }
        else { for(const std::string& busname : *buses) { busesvector.emplace_back(std::string(busname)); } }
        retnode["buses"] = jsonlib::Node{std::move(busesvector)};
        return retnode;
    }

    jsonlib::Node Parser::GetSvgNode(int arg_id) {
        //использую stringstream чтобы не нужно было тратить ресурсы на вечную переаллокацию строки
        std::stringstream svg;
        Render(svg);
        std::map<std::string , jsonlib::Node> retnode;
        retnode["request_id"] = jsonlib::Node{arg_id};
        //а тут уже никак без преобразования в строку, Node не умеет в stringstream
        retnode["map"] = jsonlib::Node{std::move(svg.str())};
        return retnode;
    }

    void Parser::Render(std::ostream& arg_output) {
        //создаем документ, который будем рендерить
        svglib::Document todraw;
        //линии путей и их названия
        std::vector<svglib::Polyline> routes;
        std::vector<svglib::Text>     routenames;
        //наполняем их
        RenderRoutes(routes, routenames);
        //то же самое для точек остановок и их названий
        std::vector<svglib::Circle> stopcircles;
        std::vector<svglib::Text>   stopnames;
        //теперь наполняем
        RenderStops(stopnames, stopcircles);
        //добавляем в правильном порядке
        for(svglib::Polyline& route : routes) { todraw.Add(std::move(route)); }
        for(svglib::Text& routename : routenames) { todraw.Add(std::move(routename)); }
        for(svglib::Circle& stopscircle : stopcircles) { todraw.Add(std::move(stopscircle)); }
        for(svglib::Text& stopname : stopnames) { todraw.Add(std::move(stopname)); }
        //рендерим todraw в arg_output
        todraw.Render(arg_output);
    }

    void Parser::RenderRoutes(std::vector<svglib::Polyline>& arg_routes , std::vector<svglib::Text>& arg_routenames) {
        //цикл с рендерингом всех путей вынесен в отдельную функцию для: симметричности в render, и для
        //более логичного RouteToPolyline, который только преобразует один путь в ломаную, а не все
        const std::vector<const common::sBus*> buses = transport_catalogue_->GetSortedBuses(true);
        for(const common::sBus* const bus : buses) {
            svglib::Polyline route;
            route.SetStrokeWidth(render_settings_.line_width).SetStrokeColor(render_settings_.GetNextColor()).
                  SetFillColor("none").SetStrokeLineCap(svglib::eStrokeLineCap::ROUND).
                  SetStrokeLineJoin(svglib::eStrokeLineJoin::ROUND);
            RouteToPolyline(bus, route, arg_routenames);
            arg_routes.push_back(std::move(route));
        }
    }

    /*template<bool tIsReverse> раз уж мой template такой антиприкольный, пусть будет двойной цикл  ¯\_ (ツ)_/¯*/
    void Parser::RouteToPolyline(const common::sBus*        arg_bus
                               , svglib::Polyline&          arg_route
                               , std::vector<svglib::Text>& arg_routenames) {
        if(!arg_bus) { return; }
        /*auto stops = [](const auto& arg_bus_stops) {
            тут я делала разветвление между вариантом когда функция была вызвана первый раз
            if constexpr(tIsReverse) { return std::ranges::reverse_view(arg_bus_stops); }
            и когда рекурсивно. используется сразу вызываемая лямбда для того, чтобы данные шли в
            одну и ту же переменную и один цикл справлялся
            else { return std::views::all(arg_bus_stops); }
        }(arg_bus->stops);*/
        auto stops     = arg_bus->stops;
        auto makelabel = [&](const common::sStop* arg_stop) {
            svglib::Text routenamebg;
            routenamebg.SetFillColor(render_settings_.underlayer_color).
                        SetStrokeWidth(render_settings_.underlayer_width).
                        SetStrokeLineCap(svglib::eStrokeLineCap::ROUND).
                        SetStrokeLineJoin(svglib::eStrokeLineJoin::ROUND).
                        SetStrokeColor(render_settings_.underlayer_color).SetFontFamily("Verdana").SetFontWeight("bold")
                       .SetData(arg_bus->name).SetFontSize(render_settings_.bus_label_font_size).
                        SetOffset(render_settings_.bus_label_offset).
                        SetPosition(geo::Geo::GeoCoordToDrawCoord(arg_stop->coordinates, map_data_, render_settings_));
            arg_routenames.push_back(std::move(routenamebg));
            svglib::Text routenamefg;
            routenamefg.SetFillColor(render_settings_.GetColor()).SetFontFamily("Verdana").SetFontWeight("bold").
                        SetData(arg_bus->name).SetFontSize(render_settings_.bus_label_font_size).
                        SetOffset(render_settings_.bus_label_offset).
                        SetPosition(geo::Geo::GeoCoordToDrawCoord(arg_stop->coordinates, map_data_, render_settings_));
            arg_routenames.push_back(std::move(routenamefg));
        };
        bool first = true;
        for(const common::sStop* stop : stops) {
            //проходимся по остановкам пути и делаем точки ломаной и названия
            if(stop == nullptr) { continue; }
            if(first) {
                makelabel(stop);
                first = false;
            }
            arg_route.AddPoint(geo::Geo::GeoCoordToDrawCoord(stop->coordinates, map_data_, render_settings_));
        }
        first = true;
        if(!arg_bus->is_roundtrip) {
            for(const common::sStop* stop : std::ranges::reverse_view(stops)) {
                if(stop == nullptr) { continue; }
                if(first) {
                    //проверяем не заканчивается ли некруговой маршрут на той же остановке, что и начался
                    if(stops.front() != stops.back()) { makelabel(stop); }
                    first = false;
                    continue;
                }
                arg_route.AddPoint(geo::Geo::GeoCoordToDrawCoord(stop->coordinates, map_data_, render_settings_));
            }
        }
    }

    void Parser::RenderStops(std::vector<svglib::Text>&   arg_stopnames
                           , std::vector<svglib::Circle>& arg_stopcircles) const {
        const std::vector<const common::sStop*> stops = transport_catalogue_->GetSortedStops(true);
        for(const common::sStop* stop : stops) {
            //проходимся по всем отсортированным остановкам с маршрутами и рисуем точки и названия
            svglib::Circle circle;
            circle.SetFillColor("white").SetCenter(geo::Geo::GeoCoordToDrawCoord(stop->coordinates
                                                                               , map_data_
                                                                               , render_settings_)).
                   SetRadius(render_settings_.stop_radius);
            arg_stopcircles.push_back(std::move(circle));
            svglib::Text stopnamebg;
            stopnamebg.SetPosition(geo::Geo::GeoCoordToDrawCoord(stop->coordinates, map_data_, render_settings_)).
                       SetOffset(render_settings_.stop_label_offset).SetFontSize(render_settings_.stop_label_font_size).
                       SetFontFamily("Verdana").SetData(stop->name).SetFillColor(render_settings_.underlayer_color).
                       SetStrokeWidth(render_settings_.underlayer_width).SetStrokeLineCap(svglib::eStrokeLineCap::ROUND)
                      .SetStrokeColor(render_settings_.underlayer_color).
                       SetStrokeLineJoin(svglib::eStrokeLineJoin::ROUND);
            arg_stopnames.push_back(std::move(stopnamebg));
            svglib::Text stopnamefg;
            stopnamefg.SetPosition(geo::Geo::GeoCoordToDrawCoord(stop->coordinates, map_data_, render_settings_)).
                       SetOffset(render_settings_.stop_label_offset).SetFontSize(render_settings_.stop_label_font_size).
                       SetFontFamily("Verdana").SetData(stop->name).SetFillColor("black");
            arg_stopnames.push_back(std::move(stopnamefg));
        }
    }
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
