/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#include <concepts>
#include <istream>
#include <map>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
namespace json {
    /*[BEGIN:================================================DECL====================================================]*/
    class Node;
    using Dict   = std::map<std::string , Node>;
    using Array  = std::vector<Node>;
    using number_TD = std::variant<int , double>;
    using Value_TD  = std::variant<std::nullptr_t , Array , Dict , bool , int , double , std::string>;

    enum eValueType { NULLPTR_T , ARRAY_TD , DICT_TD , BOOL , INT , DOUBLE , STRING };
    template<typename tFirst , typename ... tNth> concept cOneOf = (std::same_as<tFirst , tNth> || ...);
    template<typename tRecieved>concept cIsAllowedNodeType = cOneOf<
        tRecieved , std::nullptr_t , Array , Dict , bool , int , double , std::string>;

    /*[END:==================================================DECL====================================================]*/

    /*[BEGIN:================================================NODE====================================================]*/
    class Node final : private Value_TD {
        public:
            Node() = default;
            template<typename tNumber>
            Node(tNumber arg_value) requires(std::is_same_v<number_TD , tNumber>) : Value_TD{
                                                                                      std::visit([](auto&& arg_val) ->
                                                                                             Value_TD {
                                                                                                     return arg_val;
                                                                                                 }
                                                                                               , arg_value)} {}

            template<cIsAllowedNodeType tValueType>
            Node(tValueType&& arg_value) : Value_TD{std::forward<tValueType>(arg_value)} {}
            Node(const char* arg_value) : Value_TD{std::string(arg_value)} {}
            explicit Node(const Value_TD& arg_value) : Value_TD{arg_value} {}
            explicit Node(Value_TD&& arg_value) : Value_TD{std::move(arg_value)} {}

            template<cIsAllowedNodeType tValueType>
            [[nodiscard]] bool Is() const;

            template<cIsAllowedNodeType tValueType>
            std::conditional_t<std::disjunction_v<std::is_same<tValueType , double> , std::is_same<tValueType , int>> ,
                               tValueType , const tValueType&> As() const;

            template<cIsAllowedNodeType tValueType>
            tValueType& AsMutable();

            [[nodiscard]] eValueType      GetValueType() const;
            [[nodiscard]] Value_TD&       GetValue();
            [[nodiscard]] const Value_TD& GetValue() const;
            bool                          operator==(const Node& arg_other) const;
            bool                          operator!=(const Node& arg_other) const;
    };



    /*[END:=================================================NODE=====================================================]*/

    /*[BEGIN:=============================================DOCUMENT===================================================]*/
    class Document {
        public:
            Document(Node arg_root) : root_(arg_root)) {}
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

            Node ParseNode();
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
            JsonPrinter(std::ostream& arg_output) : output_{arg_output} {}
            void PrintNode(const Node& arg_node , int arg_indent);
        private:
            void          PrintString(const std::string& arg_string) const;
            void          PrintArray(const Array& arg_array , int arg_indent);
            void          PrintDict(const Dict& arg_dict , int arg_indent);
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
