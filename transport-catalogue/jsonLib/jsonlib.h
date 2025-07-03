/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <istream>
#include <map>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace jsonlib {
    /*[BEGIN:================================================DECL====================================================]*/
    class Node;
    using dict_TD   = std::map<std::string , Node>;
    using array_TD  = std::vector<Node>;
    using number_TD = std::variant<int , double>;
    using value_TD  = std::variant<std::nullptr_t , array_TD , dict_TD , bool , int , double , std::string>;
    enum eValueType { NULLPTR_T , ARRAY_TD , DICT_TD , BOOL , INT , DOUBLE , STRING };
    template<typename tRecieved> concept cIsAllowedNodeType = std::same_as<tRecieved , std::nullptr_t> || std::same_as<
        tRecieved , array_TD> || std::same_as<tRecieved , dict_TD> || std::same_as<tRecieved , bool> || std::same_as<
        tRecieved , int> || std::same_as<tRecieved , double> || std::same_as<tRecieved , std::string>;
    class ParsingError final : public std::runtime_error {
        public:
            using runtime_error::runtime_error;
    };
    /*[BEGIN:==========================================ENUM CONVERTER================================================]*/
    template<cIsAllowedNodeType tRecieved>
    struct ValueTypeMap;
    template<>
    struct ValueTypeMap<std::nullptr_t> {
        static constexpr eValueType kValue = NULLPTR_T;
    };
    template<>
    struct ValueTypeMap<array_TD> {
        static constexpr eValueType kValue = ARRAY_TD;
    };
    template<>
    struct ValueTypeMap<dict_TD> {
        static constexpr eValueType kValue = DICT_TD;
    };
    template<>
    struct ValueTypeMap<bool> {
        static constexpr eValueType kValue = BOOL;
    };
    template<>
    struct ValueTypeMap<int> {
        static constexpr eValueType kValue = INT;
    };
    template<>
    struct ValueTypeMap<double> {
        static constexpr eValueType kValue = DOUBLE;
    };
    template<>
    struct ValueTypeMap<std::string> {
        static constexpr eValueType kValue = STRING;
    };
    /*[END:============================================ENUM CONVERTER================================================]*/
    /*[END:==================================================DECL====================================================]*/

    /*[BEGIN:================================================NODE====================================================]*/
    class Node {
        public:
            // ReSharper disable CppNonExplicitConvertingConstructor
            Node() = default;

            template<cIsAllowedNodeType tValueType>
            Node(tValueType&& arg_value): value_{std::forward<tValueType>(arg_value)} {}

            Node(number_TD arg_value): value_{std::visit([]<typename tVal>(tVal&& arg_val) -> value_TD {
                                                             return arg_val;
                                                         }
                                                       , arg_value)} {}

            Node(const char* arg_value): value_{std::string(arg_value)} {}
            // ReSharper restore CppNonExplicitConvertingConstructor
            template<cIsAllowedNodeType tValueType>
            [[nodiscard]] bool Is() const {
                if constexpr(std::same_as<tValueType , double>) {
                    if(std::holds_alternative<int>(value_)) { return true; }
                }
                return std::holds_alternative<tValueType>(value_);
            }

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

            template<cIsAllowedNodeType tValueType>
            tValueType& AsMutable()  {
                if(!Is<tValueType>()) { throw std::logic_error("Unexpected node type"); }
                return std::get<tValueType>(value_);
            }

            [[nodiscard]] eValueType      GetValueType() const;
            value_TD&                     GetValue();
            [[nodiscard]] const value_TD& GetValue() const;
            bool                          operator==(const Node& arg_other) const;
            bool                          operator!=(const Node& arg_other) const;
        private:
            value_TD value_;
    };
    /*[END:=================================================NODE=====================================================]*/

    /*[BEGIN:=============================================DOCUMENT===================================================]*/
    class Document {
        public:
            // ReSharper disable CppNonExplicitConvertingConstructor
            Document(Node arg_root) : root_(std::move(arg_root)) {}
            Document(Document&& arg_other) noexcept : root_(std::move(arg_other.root_)) {}
            // ReSharper restore CppNonExplicitConvertingConstructor
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
            // ReSharper disable CppNonExplicitConvertingConstructor
            JsonReader(std::istream& arg_in): input_(arg_in)
                                          , current_{input_.get()} {}

            // ReSharper restore CppNonExplicitConvertingConstructor
            Node                  ParseNode();
            [[nodiscard]] int Peek() const;
        private:
            bool        SkipWhitespace();
            bool        Consume(bool arg_greedymode = false);
            bool        PeekExpect(char arg_character , bool arg_critical);
            void        ParseNullNBool(const std::string& arg_literal = "");
            number_TD   ParseNumber();
            std::string ParseString();
            Node        ParseArray();
            Node        ParseDict();

            std::istream& input_;
            int           current_;
    };
    class JsonPrinter {
        public:
            // ReSharper disable CppNonExplicitConvertingConstructor
            JsonPrinter(std::ostream& arg_output) : output_{arg_output} {}
            // ReSharper restore CppNonExplicitConvertingConstructor
            void PrintNode(const Node& arg_node , int arg_indent);
            // ReSharper disable CppInconsistentNaming
            // ReSharper restore CppNonExplicitConvertingConstructor
        private:
            void PrintString(const std::string& arg_string) const;
            void PrintArray(const array_TD& arg_array , int arg_indent);
            void PrintDict(const dict_TD& arg_dict , int arg_indent);

            std::ostream& output_;
    };
    Document gLoad(std::istream& arg_input);
    void     gPrint(const Document& arg_document , JsonPrinter& arg_output);
    void     gPrint(Document* arg_document , JsonPrinter& arg_output);
    void     gPrint(const Document& arg_document , std::ostream& arg_output);
    void     gPrint(Document* arg_document , std::ostream& arg_output);
    /*[END:=================================================I/O======================================================]*/
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
