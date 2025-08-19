/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
//макрос для более красивого выделения интерфейсных функций
#define PURE_CTM 0
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>
namespace svglib {
    /*[BEGIN:===============================================ENUMS====================================================]*/
    enum class eStrokeLineCap { BUTT , ROUND , SQUARE , };
    enum class eStrokeLineJoin { ARCS , BEVEL , MITER , MITER_CLIP , ROUND , };
    /*[END:=================================================ENUMS====================================================]*/

    /*[BEGIN:==============================================STRUCTS===================================================]*/
    struct sSolidColor {
        unsigned char red{0};
        unsigned char green{0};
        unsigned char blue{0};
    };
    struct sTransparentColor {
        unsigned char red{0};
        unsigned char green{0};
        unsigned char blue{0};
        double        opacity{1};
    };
    /*[BEGIN:=============================================TYPEDEFS===================================================]*/
    using color_TD = std::variant<std::monostate , std::string , sSolidColor , sTransparentColor>;
    inline const color_TD K_NONE_COLOR{"none"};
    /*[END:===============================================TYPEDEFS===================================================]*/
    struct sColourVisitor {
        std::ostream& out;
        void          operator()(std::monostate) const { out << "none"; }
        void          operator()(const std::string& arg_str) const { out << arg_str; }

        void operator()(const sSolidColor& arg_rgb) const {
            out << "rgb(" << static_cast<int>(arg_rgb.red) << ',' << static_cast<int>(arg_rgb.green) << ',' <<
                    static_cast<int>(arg_rgb.blue) << ')';
        }

        void operator()(const sTransparentColor& arg_rgba) const {
            const std::ios_base::fmtflags f(out.flags());
            out << "rgba(" << static_cast<int>(arg_rgba.red) << ',' << static_cast<int>(arg_rgba.green) << ',' <<
                    static_cast<int>(arg_rgba.blue) << ',' << arg_rgba.opacity << ')';
            out.flags(f);
        }
    };
    struct sPoint {
        sPoint() = default;

        sPoint(const double arg_x , const double arg_y)
            : x(arg_x)
          , y(arg_y) {}

        double x{0};
        double y{0};
    };
    struct sRenderContext {
        sRenderContext(std::ostream& arg_out)
            : out(arg_out) {}

        sRenderContext(std::ostream& arg_out , const int arg_indent_step , const int arg_indent = 0)
            : out(arg_out)
          , indent_step(arg_indent_step)
          , indent(arg_indent) {}

        [[nodiscard]] sRenderContext Indented() const { return {out , indent_step , indent + indent_step}; }

        void RenderIndent() const { for(int i{0} ; i < indent ; ++i) { out.put(' '); } }

        std::ostream& out;
        int           indent_step{0};
        int           indent{0};
    };
    /*[END:================================================STRUCTS===================================================]*/

    /*[BEGIN:=============================================OPERATORS==================================================]*/
    //<< для eStrokeLineCap
    inline std::ostream& operator<<(std::ostream& arg_os , const eStrokeLineCap& arg_estroke_line_cap) {
        switch(arg_estroke_line_cap) {
            case eStrokeLineCap::BUTT : arg_os << "butt";
                break;
            case eStrokeLineCap::ROUND : arg_os << "round";
                break;
            case eStrokeLineCap::SQUARE : arg_os << "square";
                break;
        }
        return arg_os;
    }
    //<< для eStrokeLineJoin
    inline std::ostream& operator<<(std::ostream& arg_os , const eStrokeLineJoin& arg_estroke_line_join) {
        switch(arg_estroke_line_join) {
            case eStrokeLineJoin::ARCS : arg_os << "arcs";
                break;
            case eStrokeLineJoin::BEVEL : arg_os << "bevel";
                break;
            case eStrokeLineJoin::MITER : arg_os << "miter";
                break;
            case eStrokeLineJoin::MITER_CLIP : arg_os << "miter-clip";
                break;
            case eStrokeLineJoin::ROUND : arg_os << "round";
                break;
        }
        return arg_os;
    }
    inline std::ostream& operator<<(std::ostream& arg_out , const color_TD& arg_colour) {
        std::visit(sColourVisitor{arg_out}, arg_colour);
        return arg_out;
    }

    /*[END:===============================================OPERATORS==================================================]*/

    /*[BEGIN:==============================================CLASSES===================================================]*/
    /*[BEGIN:==============================================OBJECTS===================================================]*/
    class Object {
        public:
            virtual ~Object() = default;
            void    Render(const sRenderContext& arg_context) const;
        private:
            virtual void RenderObject(const sRenderContext& arg_context) const = PURE_CTM;
    };
    template<typename tDerived>
    class PathProps {
        public:
            virtual ~PathProps() = default;
            tDerived& SetFillColor(const color_TD& arg_colour);
            tDerived& SetStrokeColor(const color_TD& arg_colour);
            tDerived& SetStrokeWidth(double arg_width);
            tDerived& SetStrokeLineCap(eStrokeLineCap arg_line_cap);
            tDerived& SetStrokeLineJoin(eStrokeLineJoin arg_line_join);
            void      RenderProperties(const sRenderContext& arg_context) const;
        private:
            std::optional<color_TD>       fill_colour_ , stroke_colour_;
            std::optional<double>          stroke_width_;
            std::optional<eStrokeLineCap>  line_cap_;
            std::optional<eStrokeLineJoin> line_join_;
    };
//наследует от темплейта со своим классом в темплейте
    class Circle final : public Object , public PathProps<Circle> {
        public:
            Circle& SetCenter(sPoint arg_center);
            Circle& SetRadius(double arg_radius);
        private:
            void   RenderObject(const sRenderContext& arg_context) const override;
            sPoint center_;
            double radius_{1.0};
    };

    class Polyline final : public Object , public PathProps<Polyline> {
        public:
            Polyline& AddPoint(sPoint arg_point);
        private:
            void                RenderObject(const sRenderContext& arg_context) const override;
            std::vector<sPoint> points_;
    };

    class Text final : public Object , public PathProps<Text> {
        public:
            Text& SetPosition(sPoint arg_pos);
            Text& SetOffset(sPoint arg_offset);
            Text& SetFontSize(uint32_t arg_size);
            Text& SetFontFamily(const std::string& arg_font_family);
            Text& SetFontWeight(const std::string& arg_font_weight);
            Text& SetData(const std::string& arg_data);
        private:
            void RenderObject(const sRenderContext& arg_context) const override;

            static std::ostream& RenderTextDataEscaped(std::ostream& arg_out , const std::string& arg_data) {
                for(const int8_t c : arg_data) {
                    switch(c) {
                        case '&' : arg_out << "&amp;";
                            break;
                        case '<' : arg_out << "&lt;";
                            break;
                        case '>' : arg_out << "&gt;";
                            break;
                        case '"' : arg_out << "&quot;";
                            break;
                        case '\'' : arg_out << "&apos;";
                            break;
                        default : arg_out << c;
                    }
                }
                return arg_out;
            }

            sPoint      position_ , offset_;
            unsigned    font_size_{1};
            std::string font_family_ , font_weight_ , data_;
    };
    /*[END:================================================OBJECTS===================================================]*/

    /*[BEGIN:=========================================OBJECTS_CONTAINERS=============================================]*/
    class ObjectContainer {
        public:
            virtual ~ObjectContainer() = default;

            template<typename tObjectTypename>
            void Add(tObjectTypename arg_object) { AddPtr(std::make_unique<tObjectTypename>(arg_object)); }

            virtual void AddPtr(std::unique_ptr<Object>&& arg_obj) = PURE_CTM;
    };

    class Document final : public ObjectContainer {
        public:
            void AddPtr(std::unique_ptr<Object>&& arg_obj) override;
            void Render(std::ostream& arg_out) const;
        private:
            std::vector<std::unique_ptr<Object>> objects_;
    };
    /*[END:===========================================OBJECTS_CONTAINERS=============================================]*/
    /*[END:================================================CLASSES===================================================]*/
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */