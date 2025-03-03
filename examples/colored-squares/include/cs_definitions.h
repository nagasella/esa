#ifndef CS_DEFINITIONS_H
#define CS_DEFINITIONS_H

#include "esa.h"

namespace cs
{
    using u32 = esa::u32;

    // Parametrization of an entity table and its updaters
    using entity_table   = esa::entity_table<128, 4, 4, 4, 8, 2>;
    using entity_updater = esa::entity_updater<entity_table>;
    using table_updater  = esa::table_updater<entity_table>;

    // Table fields
    namespace fields
    {
        enum
        {
            // bn::fixed
            X = 0,
            Y = 1,
            VX = 2,
            VY = 3,

            // int
            ANGLE = 0,

            // enum
            SCALE = 0, // left bit-shift
            SCALE_SZ = 2, // size of the field in bits
            ANIM_CURR = 2,
            ANIM_CURR_SZ = 2,
            ANIM_FIRST = 4,
            ANIM_FIRST_SZ = 2,
            ANIM_LAST = 6,
            ANIM_LAST_SZ = 2,
            ANIM_TIMER = 8,
            ANIM_TIMER_SZ = 4,

            // bool
            VISIBLE = 0
        };
    }

    // Entity models
    namespace models
    {
        enum
        {
            RED_SQUARE = 0,
            BLUE_SQUARE = 1,
            YELLOW_SQUARE = 2,
            FLASHING_SQUARE = 3
        };
    }

    // Updaters tags
    namespace tags
    {
        enum
        {
            MOVEMENT = 0,
            ROTATION = 1,
            SCALING = 2,
            VISIBILITY = 3,
            ENTITY_MANAGER = 4,
            ANIMATION = 5
        };
    }
    
}


#endif