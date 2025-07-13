/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#include "json.h"
#include <charconv>
#include <memory>
namespace json {
    /*[BEGIN:===============================================NODE=====================================================]*/
    eValueType Node::GetValueType() const {
        const unsigned long long index = this->index();
        switch(index) {
            case 0 : return NULLPTR_T;
            case 1 : return ARRAY_TD;
            case 2 : return DICT_TD;
            case 3 : return BOOL;
            case 4 : return INT;
            case 5 : return DOUBLE;
            case 6 : return STRING;
            default : throw std::logic_error("INCODDECT INDEX");
                //использую throw std::logic_error чтобы убрать зависимость от салата, Я ВСЕ ЕЩЕ НЕ ИИ
                //(если в этот раз проверяет не Дмитрий Мамонтов - немного контекста: он сказал что мой код
                //похож на ИИшный И Я, КАК СИДЯЩАЯ ПО 12-15 ЧАСОВ В ДЕНЬ ПЕРЕД IDE, ДО СИХ ПОР ОСКОРБЛЕНА)
                //(ну и еще у меня оч neat библиотека есть - SALAT™, которую я для профилировки написала)
        }
    }

    bool Node::operator==(const Node& arg_other) const {
        if(GetValueType() != arg_other.GetValueType()) { return false; }
        switch(GetValueType()) {
            case NULLPTR_T : return true;
            case ARRAY_TD : return As<Array>() == arg_other.As<Array>();
            case DICT_TD : return As<Dict>() == arg_other.As<Dict>();
            case BOOL : return As<bool>() == arg_other.As<bool>();
            case INT : return As<int>() == arg_other.As<int>();
            case DOUBLE : return As<double>() == arg_other.As<double>();
            case STRING : return As<std::string>() == arg_other.As<std::string>();
        }
        throw std::logic_error("INCODDECT INDEX");
    }

    bool            Node::operator!=(const Node& arg_other) const { return !(*this == arg_other); }
    Value&       Node::GetValue() { return *this; }
    const Value& Node::GetValue() const { return *this; }
    /*[END:=================================================NODE=====================================================]*/

    /*[BEGIN:=============================================DOCUMENT===================================================]*/
    const Node& Document::GetRoot() const { return root_; }
    Node&       Document::GetRoot() { return root_; }
    bool        Document::operator==(const Document& arg_other) const { return root_ == arg_other.GetRoot(); }
    bool        Document::operator!=(const Document& arg_other) const { return !(*this == arg_other); }
    /*[END:===============================================DOCUMENT===================================================]*/

    /*[BEGIN:============================================JSONREADER==================================================]*/
    /*[BEGIN:============================================JSONREADER:HELPERS==========================================]*/
    bool JsonReader::SkipWhitespace() {
        bool fail = true;
        while(current_ == ' ' || current_ == '\n' || current_ == '\t' || current_ == '\r') {
            current_ = input_.get();
            fail     = false;
        }
        return fail;
    }

    bool JsonReader::Consume(const bool arg_greedymode) {
        current_ = input_.get();
        if(arg_greedymode) { SkipWhitespace(); }
        return true;
    }

    bool JsonReader::PeekExpect(const char arg_character , const bool arg_critical) {
        SkipWhitespace();
        if(current_ != arg_character) {
            if(arg_critical) { throw std::runtime_error("PeekExpect: Expected different char"); }
            return false;
        }
        Consume(true);
        return true;
    }

    /*[END:==============================================JSONREADER:HELPERS==========================================]*/

    /*[BEGIN:============================================JSONREADER:PARSERS==========================================]*/
    Node JsonReader::ParseNode() {
        SkipWhitespace();
        Node out_node;
        switch(current_) {
            case 'n' : ParseNullNBool("null");
                out_node = nullptr;
                break;
            case 't' : ParseNullNBool("true");
                out_node = true;
                break;
            case 'f' : ParseNullNBool("false");
                out_node = false;
                break;
            case '"' : out_node = ParseString();
                break;
            case '{' : out_node = ParseDict();
                break;
            case '[' : out_node = ParseArray();
                break;
            case EOF : throw std::runtime_error("ParseNode: Unexpected end of file");
            default : if(std::isdigit(static_cast<unsigned char>(current_)) || current_ == '-') {
                    out_node = Node{ParseNumber()};
                    break;
                }
                throw std::runtime_error("ParseNode: Invalid character");
        }
        SkipWhitespace();
        return out_node;
    }

    void JsonReader::ParseNullNBool(const std::string& arg_literal) {
        for(const char ch : arg_literal) {
            if(current_ != ch) { throw std::runtime_error("Expected"); }
            Consume(false);
        }
    }

    number_TD JsonReader::ParseNumber() {
        std::string number;
        bool        is_integer = true;
        if(current_ == '-') {
            number += static_cast<int8_t>(current_);
            Consume(true);
        }
        while(isdigit(current_)) {
            number += static_cast<int8_t>(current_);
            Consume(true);
        }
        if(current_ == '.') {
            is_integer = false;
            number += '.';
            Consume(true);
            while(isdigit(current_)) {
                number += static_cast<int8_t>(current_);
                Consume(true);
            }
        }
        if(tolower(current_) == 'e') {
            is_integer = false;
            number += 'e';
            Consume(true);
            if(current_ == '+' || current_ == '-') {
                number += static_cast<int8_t>(current_);
                Consume(true);
            }
            while(isdigit(current_)) {
                number += static_cast<int8_t>(current_);
                Consume(true);
            }
        }
        auto finalize_parsing = []<typename tNumber>(const std::string& arg_number , tNumber) {
            tNumber value;
            auto    [ptr, ec] = std::from_chars(arg_number.data(), arg_number.data() + arg_number.size(), value);
            if(ec != std::errc{} || ptr != arg_number.data() + arg_number.size()) {
                throw std::runtime_error("INVALID NUMBER");
            }
            return value;
        };
        if(is_integer) { return finalize_parsing(number, int32_t{}); }
        return finalize_parsing(number, double{});
    }

    std::string JsonReader::ParseString() {
        std::string result;
        if(current_ != '"') { throw std::runtime_error("EXPECTED '\"'"); }
        Consume(false);
        while(current_ != '"' && current_ != EOF) {
            if(current_ == '\\') {
                Consume(false);
                if(current_ == EOF) { throw std::runtime_error("UNEXPECTED EOF"); }
                switch(current_) {
                    case 'n' : result += '\n';
                        break;
                    case 'r' : result += '\r';
                        break;
                    case 't' : result += '\t';
                        break;
                    case '"' : result += '"';
                        break;
                    case '\\' : result += '\\';
                        break;
                    default : throw std::runtime_error("UNRECOQNIZED ESCAPE SEQUENCE");
                }
                Consume(false);
            }
            else {
                result += static_cast<char>(current_);
                Consume(false);
            }
        }
        if(current_ != '"') { throw std::runtime_error("EXPECTED '\"'"); }
        Consume(false);
        return result;
    }

    Node JsonReader::ParseArray() {
        Array arr;
        PeekExpect('[', true);
        if(PeekExpect(']', false)) { return arr; }
        while(true) {
            arr.emplace_back(std::move(ParseNode()));
            if(PeekExpect(']', false)) { return arr; }
            PeekExpect(',', true);
            SkipWhitespace();
        }
    }

    Node JsonReader::ParseDict() {
        Dict dict;
        PeekExpect('{', true);
        if(PeekExpect('}', false)) { return dict; }

        while(true) {
            std::string key = ParseString();
            PeekExpect(':', true);
            dict.emplace(std::move(key), std::move(ParseNode()));
            if(PeekExpect('}', false)) { return dict; }
            PeekExpect(',', true);
        }
    }

    /*[END:==============================================JSONREADER:PARSERS==========================================]*/
    /*[END:==============================================JSONREADER==================================================]*/

    /*[BEGIN:============================================JSONPRINTER=================================================]*/
    void JsonPrinter::PrintNode(const Node& arg_node , const int arg_indent = 0) {
        switch(arg_node.GetValueType()) {
            case NULLPTR_T : output_ << "null";
                break;
            case ARRAY_TD : PrintArray(arg_node.As<Array>(), arg_indent);
                break;
            case DICT_TD : PrintDict(arg_node.As<Dict>(), arg_indent);
                break;
            case BOOL : output_ << (arg_node.As<bool>() ? "true" : "false");
                break;
            case INT : output_ << arg_node.As<int>();
                break;
            case DOUBLE : output_ << arg_node.As<double>();
                break;
            case STRING : PrintString(arg_node.As<std::string>());
                break;
        }
    }

    void JsonPrinter::PrintString(const std::string& arg_string) const {
        output_.put('\"');
        for(const char c : arg_string) {
            switch(c) {
                case '\r' : output_ << "\\r";
                    break;
                case '\n' : output_ << "\\n";
                    break;
                case '\t' : output_ << "\\t";
                    break;
                case '"' : [[fallthrough]];
                case '\\' : output_.put('\\');
                    [[fallthrough]];
                default : output_.put(c);
                    break;
            }
        }
        output_.put('\"');
    }

    void JsonPrinter::PrintArray(const Array& arg_array , const int arg_indent) {
        const std::string indent_str(arg_indent * 4, ' ');
        const std::string child_indent_str((arg_indent + 1) * 4, ' ');
        output_ << "[\n";
        for(size_t i = 0 ; i < arg_array.size() ; ++i) {
            output_ << child_indent_str;
            PrintNode(arg_array[i], arg_indent + 1);
            if(i < arg_array.size() - 1) { output_ << ","; }
            output_ << "\n";
        }
        output_ << indent_str << "]";
    }

    void JsonPrinter::PrintDict(const Dict& arg_dict , const int arg_indent) {
        const std::string indent_str(arg_indent * 4, ' ');
        const std::string child_indent_str((arg_indent + 1) * 4, ' ');
        output_ << "{\n";
        size_t remaining = arg_dict.size();
        for(const auto& [key, value] : arg_dict) {
            output_ << child_indent_str;
            PrintString(key);
            output_ << ": ";
            PrintNode(value, arg_indent + 1);
            --remaining;
            if(remaining != 0) { output_ << ","; }
            output_ << "\n";
        }
        output_ << indent_str << "}";
    }

    /*[END:==============================================JSONPRINTER=================================================]*/

    /*[BEGIN:==============================================GLOBAL====================================================]*/
    Document gLoad(std::istream& arg_input) {
        JsonReader json_char_reader(arg_input);
        const Node root = json_char_reader.ParseNode();
        // оно не компилит в практикуме с std::move по неведомой мне причине :skull:
        return Document{/*std::move*/(root)};
    }

    void gPrint(const Document& arg_document , JsonPrinter& arg_output) {
        arg_output.PrintNode(arg_document.GetRoot(), 0);
    }

    void gPrint(Document* arg_document , JsonPrinter& arg_output) { arg_output.PrintNode(arg_document->GetRoot(), 0); }

    void gPrint(const Document& arg_document , std::ostream& arg_output) {
        JsonPrinter json_printer{arg_output};
        json_printer.PrintNode(arg_document.GetRoot(), 0);
    }

    void gPrint(Document* arg_document , std::ostream& arg_output) {
        JsonPrinter json_printer{arg_output};
        json_printer.PrintNode(arg_document->GetRoot(), 0);
    }

    /*[END:================================================GLOBAL====================================================]*/
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
