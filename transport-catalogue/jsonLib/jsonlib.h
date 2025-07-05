/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <concepts>
#include <istream>
#include <map>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
namespace jsonlib {
    /*[BEGIN:================================================DECL====================================================]*/
    class Node;
    //TD это идентификатор всех моих собственных TypeDefов
    using dict_TD   = std::map<std::string , Node>;
    using array_TD  = std::vector<Node>;
    using number_TD = std::variant<int , double>;
    using value_TD  = std::variant<std::nullptr_t , array_TD , dict_TD , bool , int , double , std::string>;
    //enum для удобных свитчей по параметрам
    enum eValueType { NULLPTR_T , ARRAY_TD , DICT_TD , BOOL , INT , DOUBLE , STRING };
    //нда, соглашусь то что тут было было ужас, скорее всего я писала это либо в 7 утра либо 7 вечера (это одно и то же
    //время, единственная разница, что в первом случае я только проснулась, а во втором - надо было лечь спать 7 часов
    //назад. фан факт - иишки не спят и пишут быстрее чем люди, хоть и хуже, да мне было оч обидно :3)
    template<typename tFirst , typename ... tNth> concept cOneOf = (std::same_as<tFirst , tNth> || ...);
    //t = template parameter which was Recieved функцией
    //все эти темплейты существуют для превентивного ограничения попытки доступа с типом, которого и быть не может
    template<typename tRecieved>concept cIsAllowedNodeType = cOneOf<
        tRecieved , std::nullptr_t , array_TD , dict_TD , bool , int , double , std::string>;

    /*[END:==================================================DECL====================================================]*/

    /*[BEGIN:================================================NODE====================================================]*/
    class Node {
        public:
            //пустой конструктор
            Node() = default;
            //создает Node из arg_value, cIsAllowedNodeType превентит попытку создания из неразрешенного типа
            template<cIsAllowedNodeType tValueType>
            Node(tValueType&& arg_value): value_{std::forward<tValueType>(arg_value)} {}

            //создает Node из number_TD, является скорее хелпером для одной из парсящих функций дальше
            //потому что она одна отвечает сразу и за double и за int чтобы избежать дублирования
            Node(number_TD arg_value): value_{std::visit([]<typename tVal>(tVal&& arg_val) -> value_TD {
                                                             return arg_val;
                                                         }
                                                       , arg_value)} {}

            //ну тут самообъясняющий конструктор, помогает избежать конфликтов между "const char" и std::string("string")
            Node(const char* arg_value): value_{std::string(arg_value)} {}
            //проверяет тип хранящейся переменной
            template<cIsAllowedNodeType tValueType>
            [[nodiscard]] bool Is() const {
                if constexpr(std::same_as<tValueType , double>) {
                    if(std::holds_alternative<int>(value_)) { return true; }
                }
                return std::holds_alternative<tValueType>(value_);
            }

            //возвращает константную версию значения, не дает использовать невозможные значения
            //и преобразует int в double если это нужно
            template<cIsAllowedNodeType tValueType>
            std::conditional_t<std::disjunction_v<std::is_same<tValueType , double> , std::is_same<tValueType , int>> ,
                               tValueType , const tValueType&> As() const {
                if(!Is<tValueType>()) { throw std::logic_error("Unexpected node type"); }
                if constexpr(std::same_as<tValueType , double>) {
                    if(std::holds_alternative<int>(value_)) { return static_cast<double>(std::get<int>(value_)); }
                    return std::get<double>(value_);
                }
                return std::get<tValueType>(value_);
            }

            //возвращает неконстантную ссылку на значение, не дает использовать невозможные значения
            //и позволяет редактировать значение которое хранится в value_
            template<cIsAllowedNodeType tValueType>
            tValueType& AsMutable() {
                if(!Is<tValueType>()) { throw std::logic_error("Unexpected node type"); }
                return std::get<tValueType>(value_);
            }

            //возвращает enum типа, nodiscard потому что главное это значение которое оно возвращает
            [[nodiscard]] eValueType GetValueType() const;
            //возвращают прямую ссылку на value_ в чистом виде
            [[nodiscard]] value_TD&       GetValue();
            [[nodiscard]] const value_TD& GetValue() const;
            //сравнивает duh
            bool operator==(const Node& arg_other) const;
            bool operator!=(const Node& arg_other) const;
        private:
            value_TD value_;
    };
    /*[END:=================================================NODE=====================================================]*/

    /*[BEGIN:=============================================DOCUMENT===================================================]*/
    class Document {
        public:
            //обертка для корневой Node, хз зачем, но задания сказали нада
            Document(Node arg_root) : root_(std::move(arg_root)) {}
            Document(Document&& arg_other) noexcept : root_(std::move(arg_other.root_)) {}
            Node&                     GetRoot();
            [[nodiscard]] const Node& GetRoot() const;
            bool                      operator==(const Document& arg_other) const;
            bool                      operator!=(const Document& arg_other) const;
        private:
            Node root_;
    };
    /*[END:===============================================DOCUMENT===================================================]*/

    /*[BEGIN:===============================================I/O======================================================]*/
    class JsonReader {
        public:
            JsonReader(std::istream& arg_in): input_(arg_in)
                                          , current_{input_.get()} {}

            //преобразует intput_ в JSON,  по сути распределитель данных между остальными
            Node ParseNode();
        private:
            //пропускает whitespace в input_
            bool              SkipWhitespace();
            //сдвигает current_ вперед (ии не делают отсылки на сатисфактори. CONSUME.)
            bool              Consume(bool arg_greedymode = false);
            //проверяет тот ли char что и arg_character сейчас current_. кидат с critical
            bool              PeekExpect(char arg_character , bool arg_critical);
            //парсит параметры которые записаны текстом
            void              ParseNullNBool(const std::string& arg_literal = "");
            //парсит номер
            number_TD         ParseNumber();
            //парсит строку
            std::string       ParseString();
            //парсит вектор через ParseNode (оч рекурсивное)
            Node              ParseArray();
            //теж самое но со словарем
            Node              ParseDict();

            std::istream& input_;
            int           current_;
    };
    class JsonPrinter {
        public:
            JsonPrinter(std::ostream& arg_output) : output_{arg_output} {}
            //парсит Node, следуя прекрасной традиции из ParseNode - опять распределитель
            void PrintNode(const Node& arg_node , int arg_indent);
        private:
            //печатает строку
            void PrintString(const std::string& arg_string) const;
            //вектор
            void PrintArray(const array_TD& arg_array , int arg_indent);
            //словарь
            void PrintDict(const dict_TD& arg_dict , int arg_indent);

            std::ostream& output_;
    };
    //загружает документ из стрима
    Document gLoad(std::istream& arg_input);
    //печатает документ в стрим
    void     gPrint(const Document& arg_document , JsonPrinter& arg_output);
    void     gPrint(Document* arg_document , JsonPrinter& arg_output);
    void     gPrint(const Document& arg_document , std::ostream& arg_output);
    void     gPrint(Document* arg_document , std::ostream& arg_output);
    /*[END:=================================================I/O======================================================]*/
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
