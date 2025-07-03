#include "svglib.h"

#include <iomanip>
namespace svglib {
    using namespace std::literals;
    /*[BEGIN:==============================================OBJECT====================================================]*/

    void Object::Render(const sRenderContext& arg_context) const {
        arg_context.RenderIndent();
        RenderObject(arg_context);
        arg_context.out << std::endl;
    }

    /*[END:================================================OBJECT====================================================]*/

    /*[BEGIN:=============================================PATHPROPS==================================================]*/

    template<typename tDerived>
    tDerived& PathProps<tDerived>::SetFillColor(const color_TD& arg_colour) {
        fill_colour_ = arg_colour;
        return static_cast<tDerived&>(*this);
    }

    template<typename tDerived>
    tDerived& PathProps<tDerived>::SetStrokeColor(const color_TD& arg_colour) {
        stroke_colour_ = arg_colour;
        return static_cast<tDerived&>(*this);
    }

    template<typename tDerived>
    tDerived& PathProps<tDerived>::SetStrokeWidth(double arg_width) {
        stroke_width_ = arg_width;
        return static_cast<tDerived&>(*this);
    }

    template<typename tDerived>
    tDerived& PathProps<tDerived>::SetStrokeLineCap(eStrokeLineCap arg_line_cap) {
        line_cap_ = arg_line_cap;
        return static_cast<tDerived&>(*this);
    }

    template<typename tDerived>
    tDerived& PathProps<tDerived>::SetStrokeLineJoin(eStrokeLineJoin arg_line_join) {
        line_join_ = arg_line_join;
        return static_cast<tDerived&>(*this);
    }

    template<typename tDerived>
    void PathProps<tDerived>::RenderProperties(const sRenderContext& arg_context) const {
        if(fill_colour_) { arg_context.out << " fill=\"" << fill_colour_.value() << "\""; }
        if(stroke_colour_) { arg_context.out << " stroke=\"" << stroke_colour_.value() << "\""; }
        if(stroke_width_) { arg_context.out << " stroke-width=\"" << stroke_width_.value() << "\""; }
        if(line_cap_) { arg_context.out << " stroke-linecap=\"" << line_cap_.value() << "\""; }
        if(line_join_) { arg_context.out << " stroke-linejoin=\"" << line_join_.value() << "\""; }
    }

    /*[END:===============================================PATHPROPS==================================================]*/

    /*[BEGIN:==============================================CIRCLE====================================================]*/

    Circle& Circle::SetCenter(const sPoint arg_center) {
        center_ = arg_center;
        return *this;
    }

    Circle& Circle::SetRadius(const double arg_radius) {
        radius_ = arg_radius;
        return *this;
    }

    void Circle::RenderObject(const sRenderContext& arg_context) const {
        arg_context.RenderIndent();
        arg_context.out << "<circle cx=\"" << center_.x << "\" cy=\"" << center_.y << "\" r=\"" << radius_ << "\"";
        RenderProperties(arg_context);
        arg_context.out << "/>";
    }

    /*[END:================================================CIRCLE===================================================]*/

    /*[BEGIN:=============================================POLYLINE===================================================]*/

    Polyline& Polyline::AddPoint(const sPoint arg_point) {
        points_.push_back(arg_point);
        return *this;
    }

    void Polyline::RenderObject(const sRenderContext& arg_context) const {
        arg_context.RenderIndent();
        if(points_.empty()) {
            arg_context.out << "<polyline points=\"\" />";
            return;
        }
        arg_context.out << "<polyline points=\"";
        for(size_t i = 0 ; i < points_.size() ; ++i) {
            if(i > 0) { arg_context.out << " "; }
            arg_context.out << points_[i].x << "," << points_[i].y;
        }
        arg_context.out << "\"";
        RenderProperties(arg_context);
        arg_context.out << "/>";
    }

    /*[END:===============================================POLYLINE===================================================]*/

    /*[BEGIN:===============================================TEXT=====================================================]*/
    Text& Text::SetPosition(const sPoint arg_pos) {
        position_ = arg_pos;
        return *this;
    }

    Text& Text::SetOffset(const sPoint arg_offset) {
        offset_ = arg_offset;
        return *this;
    }

    Text& Text::SetFontSize(const uint32_t arg_size) {
        font_size_ = arg_size;
        return *this;
    }

    Text& Text::SetFontFamily(const std::string& arg_font_family) {
        font_family_ = arg_font_family;
        return *this;
    }

    Text& Text::SetFontWeight(const std::string& arg_font_weight) {
        font_weight_ = arg_font_weight;
        return *this;
    }

    Text& Text::SetData(const std::string& arg_data) {
        data_ = arg_data;
        return *this;
    }

    void Text::RenderObject(const sRenderContext& arg_context) const {
        arg_context.RenderIndent();
        arg_context.out << "<text";
        RenderProperties(arg_context);
        arg_context.out << " x=\"" << position_.x << "\"";
        arg_context.out << " y=\"" << position_.y << "\"";
        arg_context.out << " dx=\"" << offset_.x << "\"";
        arg_context.out << " dy=\"" << offset_.y << "\"";
        arg_context.out << " font-size=\"" << font_size_ << "\"";
        if(!font_family_.empty()) { arg_context.out << " font-family=\"" << font_family_ << "\""; }
        if(!font_weight_.empty()) { arg_context.out << " font-weight=\"" << font_weight_ << "\""; }
        arg_context.out << ">";
        RenderTextDataEscaped(arg_context.out, data_);
        arg_context.out << "</text>";
    }

    /*[END:=================================================TEXT=====================================================]*/

    /*[BEGIN:=============================================DOCUMENT===================================================]*/
    void Document::Render(std::ostream& arg_out) const {
        arg_out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
        arg_out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
        const sRenderContext render_context(arg_out, 2, 1);
        for(const std::unique_ptr<Object>& object : objects_) { object->Render(render_context); }
        arg_out << "</svg>"sv;
    }

    void Document::AddPtr(std::unique_ptr<Object>&& arg_obj) { objects_.push_back(std::move(arg_obj)); }
    /*[END:===============================================DOCUMENT===================================================]*/
}
namespace shapes {
    /*[BEGIN:============================================DRAWABLE_P2=================================================]*/
    Star::Star(const svglib::sPoint& arg_center
             , const double          arg_outer_radius
             , const double          arg_inner_radius
             , const int             arg_num_rays) : center_(arg_center)
                                                 , outer_radius_{arg_outer_radius}
                                                 , inner_radius_{arg_inner_radius}
                                                 , num_rays_{arg_num_rays} {}

    void Star::Draw(svglib::ObjectContainer& arg_container) const {
        svglib::Polyline polyline;
        for(int i = 0 ; i <= num_rays_ ; ++i) {
            double angle = 2 * M_PI * (i % num_rays_) / num_rays_;
            polyline.AddPoint({center_.x + outer_radius_ * sin(angle) , center_.y - outer_radius_ * cos(angle)});
            if(i == num_rays_) { break; }
            angle += M_PI / num_rays_;
            polyline.AddPoint({center_.x + inner_radius_ * sin(angle) , center_.y - inner_radius_ * cos(angle)});
        }
        arg_container.Add(polyline.SetFillColor(svglib::color_TD{"red"}).SetStrokeColor(svglib::color_TD{"black"}));
    }

    Snowman::Snowman(const svglib::sPoint arg_head_center , const double arg_head_radius) : head_center_
                                                                                          {arg_head_center}
                                                                                        , head_radius_{
                                                                                              arg_head_radius} {}

    void Snowman::Draw(svglib::ObjectContainer& arg_container) const {
        const double r = head_radius_;
        const double x = head_center_.x;
        const double y = head_center_.y;

        const double dyc = r * 2;
        const double dyb = dyc + dyc * 1.5;

        svglib::Circle head;
        head.SetCenter(head_center_).SetRadius(r).SetFillColor(svglib::color_TD{svglib::sSolidColor{240 , 240 , 240}}).
             SetStrokeColor(svglib::color_TD{"black"});

        svglib::Circle center;
        center.SetCenter({x , y + dyc}).SetRadius(r * 1.5).
               SetFillColor(svglib::color_TD{svglib::sSolidColor{240 , 240 , 240}}).
               SetStrokeColor(svglib::color_TD{"black"});

        svglib::Circle bottom;
        bottom.SetCenter({x , y + dyb}).SetRadius(r * 2).SetFillColor(svglib::color_TD{
                                                                          svglib::sSolidColor{240 , 240 , 240}}).
               SetStrokeColor(svglib::color_TD{"black"});

        arg_container.Add(bottom);
        arg_container.Add(center);
        arg_container.Add(head);
    }

    Triangle::Triangle(const svglib::sPoint arg_point1
                     , const svglib::sPoint arg_point2
                     , const svglib::sPoint arg_point3) : point1_{arg_point1}
                                                      , point2_{arg_point2}
                                                      , point3_{arg_point3} {}

    void Triangle::Draw(svglib::ObjectContainer& arg_container) const {
        arg_container.Add(svglib::Polyline().AddPoint(point1_).AddPoint(point2_).AddPoint(point3_).AddPoint(point1_));
    }

    /*[END:==============================================DRAWABLE_P2=================================================]*/
}
template class svglib::PathProps<svglib::Circle>;
template class svglib::PathProps<svglib::Polyline>;
template class svglib::PathProps<svglib::Text>;
