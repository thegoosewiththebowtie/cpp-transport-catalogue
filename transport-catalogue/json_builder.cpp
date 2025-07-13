#include "json_builder.h"

namespace json {
    Builder::Builder() : nodes_stack_{&root_} {}

    Node Builder::Build() {
        if(!nodes_stack_.empty()) { throw std::logic_error("UNEXPECTED BUILD"); }
        return std::move(root_);
    }

    Builder::DictValueContext Builder::BeginDict() {
        AddValue(Dict{}, false);
        return *this;
    }

    Builder::BaseBuilder Builder::EndDict() {
        if(!std::holds_alternative<Dict>(GetCurrentValue())) { throw std::logic_error("UNEXPECTED ENDDICT"); }
        nodes_stack_.pop_back();
        return *this;
    }

    Builder::ArrayValueContext Builder::BeginArray() {
        AddValue(Array{}, false);
        return *this;
    }

    Builder::BaseBuilder Builder::EndArray() {
        if(!std::holds_alternative<Array>(GetCurrentValue())) { throw std::logic_error("UNEXPECTED ENDARRAY"); }
        nodes_stack_.pop_back();
        return *this;
    }

    Builder::DictKeyContext Builder::Key(std::string arg_key) {
        if(!std::holds_alternative<Dict>(GetCurrentValue())) { throw std::logic_error("UNEXPECTED KEY"); }
        nodes_stack_.push_back(&std::get<Dict>(GetCurrentValue())[std::move(arg_key)]);
        return *this;
    }

    Builder::BaseBuilder Builder::Value(json::Value arg_value) {
        AddValue(std::move(arg_value), true);
        return *this;
    }

    Value& Builder::GetCurrentValue() {
        if(nodes_stack_.empty()) { throw std::logic_error("EMPTY STACK"); }
        return nodes_stack_.back()->GetValue();
    }

    const Value& Builder::GetCurrentValue() const {
        if(nodes_stack_.empty()) { throw std::logic_error("EMPTY STACK"); }
        return nodes_stack_.back()->GetValue();
    }

    void Builder::AddValue(json::Value arg_value , const bool arg_once) {
        json::Value& cur_value = GetCurrentValue();
        if(std::holds_alternative<Array>(cur_value)) {
            json::Node& node = std::get<Array>(cur_value).emplace_back(std::move(arg_value));

            if(!arg_once) { nodes_stack_.push_back(&node); }
        }
        else {
            if(!std::holds_alternative<std::nullptr_t>(GetCurrentValue())) {
                throw std::logic_error("NON EMPTY");
            }

            cur_value = std::move(arg_value);
            if(arg_once) { nodes_stack_.pop_back(); }
        }
    }
}
