/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#include "domain.h"
namespace transport_catalogue::common {
    const svglib::color_TD& sRenderSettings::GetNextColor() {
        if(color_palette_.empty()) { return kNull; }
        if(current_id_ == color_palette_.size() - 1) { current_id_ = 0; }
        else { ++current_id_; }
        return color_palette_[current_id_ == 0 ? color_palette_.size() - 1 : current_id_ - 1];
    }

    const svglib::color_TD& sRenderSettings::GetColor() const {
        if(color_palette_.empty()) { return kNull; }
        return color_palette_[current_id_ == 0 ? color_palette_.size() - 1 : current_id_ - 1];
    }
}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
