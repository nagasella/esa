#ifndef CS_DEFINITIONS_H
#define CS_DEFINITIONS_H

#include "esa.h"

namespace cs
{
    // Parametrization of an entity table and its updaters
    using entity_table   = esa::entity_table<128, 3, 4, 4, 4, 4>;
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
            SCALE = 0, 
            SCALE_SZ = 2, // (size in bits)

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
            YELLOW_SQUARE = 2
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
            ENTITY_MANAGER = 4
        };
    }
    
}


#endif