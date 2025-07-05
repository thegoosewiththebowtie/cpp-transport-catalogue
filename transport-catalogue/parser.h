/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <iomanip>
#include <iosfwd>
#include <map>
#include <memory>
#include <stack>
#include <vector>

#include "common.h"
namespace jsonlib {
    class Node;
}
namespace transport_catalogue {
    class TransportCatalogue;
    //объединила все в один класс и файл, теперь проще будет (раньше тренажер требовал разные)
    namespace parser {
        class Parser {
            public:
                //инициализирует parser с указателем на TransportCatalogue
                Parser(TransportCatalogue* arg_transport_catalogue) : transport_catalogue_(arg_transport_catalogue) {};
                //меняет указатель на TransportCatalogue и очищает переменные
                void ChangeCatalogue(TransportCatalogue* arg_transport_catalogue);
                //десериализует Json документ поданный в arg_istream и записывает его в transport_catalogue_
                void Deserialize(std::istream& arg_istream);
                //сериализует данные из transport_catalogue_ в Json документ
                void Serialize(std::ostream& arg_output , common::eOutputFilter);
            private:
                //преобразует arg_busmap в данные необходимые для TransportCatalogue::AddBus и вызывает этот метод
                void AddNodeBus(const std::map<std::string , jsonlib::Node>& arg_busmap) const;
                //теж самое но для TransportCatalogue::AddStop
                void AddNodeStop(const std::map<std::string, jsonlib::Node>& arg_stopmap) const;
                //вытаскивает настройки из arg_render_settings и преваращает их в sRenderSettings, ведь jsonlib не
                //возвращает никаких специфичных классов, а только Nodes которые еще нужно обработать для данных,
                //которые нужны дальше, не делать же жсонскую библиотеку специфичной для одного проекта да.
                void SetRenderSettingsFromNode(const std::map<std::string , jsonlib::Node>& arg_render_settings);
                //преобразует sBus в Node, duh nodiscardы потому что данные функции не имеют смысла без возвращаемых значений
                [[nodiscard]] jsonlib::Node BusToNode(int arg_id , const std::string& arg_bus_name) const;
                //теж самое но для остановки
                [[nodiscard]] jsonlib::Node StopToNode(int arg_id , const std::string& arg_stop_name) const;
                //запускает процесс рендеринга и переносит svg в node
                [[nodiscard]] jsonlib::Node GetSvgNode(int arg_id);
                //рендерит svg в arg_output
                void Render(std::ostream& arg_output);
                //преобразует node в цвет подходящего типа
                [[nodiscard]] static svglib::color_TD NodeToColor(const jsonlib::Node& arg_node);
                //преобразует node в точку
                [[nodiscard]] static svglib::sPoint NodeToPoint(const jsonlib::Node& arg_node);
                //рендерит все маршруты
                void                                RenderRoutes(std::vector<svglib::Polyline>& arg_routes
                                                               , std::vector<svglib::Text>&
                                                                 arg_routenames);
                //рендерит один маршрут
                void RouteToPolyline(const common::sBus*        arg_bus
                                   , svglib::Polyline&          arg_route
                                   , std::vector<svglib::Text>& arg_routenames);
                //рендерит все остановки
                void RenderStops(std::vector<svglib::Text>&   arg_stopnames
                               , std::vector<svglib::Circle>& arg_stopcircles) const;
                //хранит указатель на основной transport_catalogue
                TransportCatalogue*          transport_catalogue_;
                //хранит данные необходимые для отрисовки карты
                common::sMapData        map_data_{};
                //хранит настройки рендеринга
                common::sRenderSettings render_settings_{};
                //хранит все запросы
                common::sRequests       requests_{};
        };
    }}

/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
