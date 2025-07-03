/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
#pragma once
#define SALAT_ENABLED 1
#include <string>
#include <vector>
#include "geo.h"

namespace transport_catalogue :: common {
    /*[BEGIN:==============================================STRUCTS===================================================]*/
    struct sStop {
        std::string           name;
        geo::Geo::Coordinates coordinates;
    };
    struct sBus {
        std::string         name;
        std::vector<sStop*> stops;
        bool                is_roundtrip{};
    };
    struct sBusStats {
        size_t stop_count;
        size_t unique_stop_count;
        double    factual_distance;
        double curvature;
    };
    /*[END:================================================STRUCTS===================================================]*/

    /*[BEGIN:==========================================ERROR_HANDLING================================================]*/

}
/* [≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡▲≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡≡] */
