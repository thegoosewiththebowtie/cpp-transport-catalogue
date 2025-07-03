/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#include "jsonlib.h"

#include <concepts>

#include "../common.h"
#include "../salatLib/salatlib.h"

namespace jsonlib {
    /*[BEGIN:===============================================NODE=====================================================]*/
    eValueType Node::GetValueType() const {
        return std::visit([]<typename tVal>(tVal&&) -> eValueType { return ValueTypeMap<std::decay_t<tVal>>::kValue; }
                        , value_);
    }

    bool operator==(const value_TD& arg_lhs , const value_TD& arg_rhs) {
        if(arg_lhs.index() != arg_rhs.index()) { return false; }

        return std::visit([]<typename tType>(tType&& arg_a , auto&& arg_b) -> bool {
                              if constexpr(!std::is_same_v<
                                  std::decay_t<decltype(arg_a)> , std::decay_t<decltype(arg_b)>>) { return false; }
                              else if constexpr(std::is_same_v<std::decay_t<tType> , array_TD>) {
                                  if(arg_a.size() != arg_b.size()) { return false; }
                                  for(size_t i = 0 ; i < arg_a.size() ; ++i) {
                                      if(!(arg_a[i] == arg_b[i])) { return false; }
                                  }
                                  return true;
                              }
                              else if constexpr(std::is_same_v<std::decay_t<tType> , dict_TD>) {
                                  if(arg_a.size() != arg_b.size()) { return false; }
                                  auto it1 = arg_a.begin();
                                  auto it2 = arg_b.begin();
                                  for(; it1 != arg_a.end() && it2 != arg_b.end() ; ++it1, ++it2) {
                                      if(it1->first != it2->first) { return false; }
                                      if(!(it1->second == it2->second)) { return false; }
                                  }
                                  return true;
                              }
                              else if constexpr(std::is_same_v<std::decay_t<tType> , number_TD>) {
                                  return std::visit([](auto&& arg_n1 , auto&& arg_n2) { return arg_n1 == arg_n2; }
                                                  , arg_a
                                                  , arg_b);
                              }
                              else { return arg_a == arg_b; }
                          }
                        , arg_lhs
                        , arg_rhs);
    }

    value_TD&       Node::GetValue() { return value_; }
    const value_TD& Node::GetValue() const { return value_; }
    bool            Node::operator==(const Node& arg_other) const { return (value_ == arg_other.value_); }
    bool            Node::operator!=(const Node& arg_other) const { return !(*this == arg_other); }
    /*[END:=================================================NODE=====================================================]*/

    /*[BEGIN:=============================================DOCUMENT===================================================]*/
    const Node& Document::GetRoot() const { return root_; }
    Node&       Document::GetRoot() { return root_; }
    bool        Document::operator==(const Document& arg_other) const { return root_ == arg_other.GetRoot(); }
    bool        Document::operator!=(const Document& arg_other) const { return !(*this == arg_other); }
    /*[END:===============================================DOCUMENT===================================================]*/

    /*[BEGIN:============================================JSONREADER==================================================]*/
    /*[BEGIN:============================================JSONREADER:HELPERS==========================================]*/
    int JsonReader::Peek() const { return current_; }

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
            if(arg_critical) { throw ParsingError("PeekExpect: Expected different char"); }
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
            case EOF : throw ParsingError("ParseNode: Unexpected end of file");
            default : if(std::isdigit(static_cast<unsigned char>(current_)) || current_ == '-') {
                    out_node = Node{ParseNumber()};
                    break;
                }
                throw ParsingError("ParseNode: Invalid character");
        }
        SkipWhitespace();
        SALAT_CTMFBREAK("ParseNode");
        return out_node;
    }

    void JsonReader::ParseNullNBool(const std::string& arg_literal) {
        for(const char ch : arg_literal) {
            if(current_ != ch) { throw ParsingError("Expected"); }
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
                throw ParsingError("SubParseNumber: Invalid number");
            }
            return value;
        };
        if(is_integer) { return finalize_parsing(number, int32_t{}); }
        return finalize_parsing(number, double{});
    }

    std::string JsonReader::ParseString() {
        std::string result;
        if(current_ != '"') { throw ParsingError("ParseString: string must begin with '\"'"); }
        Consume(false);
        while(current_ != '"' && current_ != EOF) {
            if(current_ == '\\') {
                Consume(false);
                if(current_ == EOF) { throw ParsingError("ParseString: unexpected EOF after backslash"); }
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
                    default : throw ParsingError("ParseString: unrecognized escape sequence");
                }
                Consume(false);
            }
            else {
                result += static_cast<char>(current_);
                Consume(false);
            }
        }
        if(current_ != '"') { throw ParsingError("ParseString: unclosed string"); }
        Consume(false);
        return result;
    }

    Node JsonReader::ParseArray() {
        array_TD arr;
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
        dict_TD dict;
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
            case ARRAY_TD : PrintArray(arg_node.As<array_TD>(), arg_indent);
                break;
            case DICT_TD : PrintDict(arg_node.As<dict_TD>(), arg_indent);
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
        SALAT_BREAK();
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

    void JsonPrinter::PrintArray(const array_TD& arg_array , const int arg_indent) {
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

    void JsonPrinter::PrintDict(const dict_TD& arg_dict , const int arg_indent) {
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
        SALAT_SALTBEGIN();
        SALAT_CTMFBEGIN("ParseNode");
        JsonReader json_char_reader(arg_input);
        Node       root = json_char_reader.ParseNode();
        // ReSharper disable once CppDFAConstantConditions
        if(json_char_reader.Peek() != -1) {
            std::cout << "gLoad: Expected EOF, got:" << json_char_reader.Peek() << std::endl;
        }
        SALAT_CTMFEND("ParseNode");
        SALAT_SALTEND();
        // оно не компилит в практикуме с std::move по неведомой мне причине :skull:
        return Document{/*std::move*/(root)};
    }

    void gPrint(const Document& arg_document , JsonPrinter& arg_output) {
        SALAT_SALTBEGIN();
        SALAT_CTMFBEGIN("PrintNode");
        arg_output.PrintNode(arg_document.GetRoot(), 0);
        SALAT_CTMFEND("PrintNode");
        SALAT_SALTEND();
    }

    void gPrint(Document* arg_document , JsonPrinter& arg_output) {
        SALAT_SALTBEGIN();
        SALAT_CTMFBEGIN("PrintNode");
        arg_output.PrintNode(arg_document->GetRoot(), 0);
        SALAT_CTMFEND("PrintNode");
        SALAT_SALTEND();
    }

    void gPrint(const Document& arg_document , std::ostream& arg_output) {
        SALAT_SALTBEGIN();
        SALAT_CTMFBEGIN("PrintNode");
        JsonPrinter json_printer{arg_output};
        json_printer.PrintNode(arg_document.GetRoot(), 0);
        SALAT_CTMFEND("PrintNode");
        SALAT_SALTEND();
    }

    void gPrint(Document* arg_document , std::ostream& arg_output) {
        SALAT_SALTBEGIN();
        SALAT_CTMFBEGIN("PrintNode");
        JsonPrinter json_printer{arg_output};
        json_printer.PrintNode(arg_document->GetRoot(), 0);
        SALAT_CTMFEND("PrintNode");
        SALAT_SALTEND();
    }

    /*[END:================================================GLOBAL====================================================]*/
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
