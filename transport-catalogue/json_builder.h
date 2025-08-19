#pragma once

#include "json.h"

namespace json {
    class Builder {
        class BaseBuilder;
        class DictKeyContext;
        class DictValueContext;
        class ArrayValueContext;

        Node               root_;
        std::vector<Node*> nodes_stack_;

        [[nodiscard]] Value_TD&       GetCurrentValue();
        [[nodiscard]] const Value_TD& GetCurrentValue() const;

        void AddValue(Value_TD arg_value , bool arg_once);

        class BaseBuilder {
            public:
                BaseBuilder(Builder& arg_builder) : builder_(arg_builder) {}
                Node Build() { return builder_.Build(); }
                DictKeyContext Key(std::string arg_key) { return builder_.Key(std::move(arg_key)); }
                BaseBuilder Value(Value_TD arg_value) { return builder_.Value(std::move(arg_value)); }
                DictValueContext BeginDict() { return builder_.BeginDict(); }
                ArrayValueContext BeginArray() { return builder_.BeginArray(); }
                BaseBuilder EndDict() { return builder_.EndDict(); }
                BaseBuilder EndArray() { return builder_.EndArray(); }
            private:
                Builder& builder_;
        };

        class DictValueContext : public BaseBuilder {
            public:
                DictValueContext(Builder& arg_builder) : BaseBuilder(arg_builder) {}
                explicit          DictValueContext(const BaseBuilder arg_context) : BaseBuilder(arg_context) {}
                Node              Build()                      = delete;
                BaseBuilder       Value(json::Value_TD arg_value) = delete;
                DictValueContext  BeginDict()                  = delete;
                ArrayValueContext BeginArray()                 = delete;
                BaseBuilder       EndArray()                   = delete;
        };

        class DictKeyContext : public BaseBuilder {
            public:
                DictKeyContext(Builder& arg_builder) : BaseBuilder(arg_builder) {}
                explicit DictKeyContext(const BaseBuilder arg_context) : BaseBuilder(arg_context) {}

                DictValueContext Value(json::Value_TD arg_value) {
                    return DictValueContext(BaseBuilder::Value(std::move(arg_value)));
                }

                Node           Build()                  = delete;
                BaseBuilder    EndDict()                = delete;
                BaseBuilder    EndArray()               = delete;
                DictKeyContext Key(std::string arg_key) = delete;
        };

        class ArrayValueContext : public BaseBuilder {
            public:
                ArrayValueContext(Builder& arg_builder) : BaseBuilder(arg_builder) {}
                explicit ArrayValueContext(const BaseBuilder arg_context) : BaseBuilder(arg_context) {}

                ArrayValueContext Value(json::Value_TD arg_value) {
                    return ArrayValueContext(BaseBuilder::Value(std::move(arg_value)));
                }

                Node           Build()                  = delete;
                DictKeyContext Key(std::string arg_key) = delete;
                BaseBuilder    EndDict()                = delete;
        };
        public:
            Builder();
            Node              Build();
            DictKeyContext    Key(std::string arg_key);
            BaseBuilder       Value(Value_TD arg_value);
            DictValueContext  BeginDict();
            BaseBuilder       EndDict();
            ArrayValueContext BeginArray();
            BaseBuilder       EndArray();
    };
}
