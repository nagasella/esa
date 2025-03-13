#ifndef CS_QUERIES_H
#define CS_QUERIES_H

#include "cs_definitions.h"

namespace cs::functions
{
    // queries
    bool find_red_squares(entity_table& table, entity e);

    struct x_boundaries
    {
        bn::fixed min, max;
    };
    bool find_yellow_squares_within(entity_table& table, entity e, x_boundaries& boundaries);

    // applied functions
    bool destroy_first_blue_square(entity_table& table, entity e);
    bool incr_blue_squares_velocity(entity_table& table, entity e);
    
}

#endif