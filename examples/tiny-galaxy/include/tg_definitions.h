#ifndef TG_DEFINITIONS_H
#define TG_DEFINITIONS_H

#include "esa.h"

namespace tg
{

    using entity = esa::entity;
    using entity_model = esa::entity_model;
    using uintn_t = esa::uintn_t;
    using entity_table = esa::entity_table<128, 4, 5, 0>;
    using entity_updater = esa::entity_updater<entity_table>;
    using table_updater = esa::table_updater<entity_table>;
    using cached_apply = esa::cached_apply<entity_table>;

    namespace fields
    {
        enum
        {
            // bn::fixed fields
            X = 0, // relative coordinates of object, with respect to parent
            Y = 1,
            DISTANCE = 2, // distance from parent object
            ANGLE = 3, // angular coordinate
            VANGULAR = 4, // angular velocity

            // uintn_t fields
            PARENT = 0, // tells the parent node of the entity in the scene-graph
            PARENT_SZ = 8 // 8 bits are sufficient to store an entity ID
        };
    }
    
    namespace tags
    {
        enum
        {
            SCENEGRAPH = 0,
            ORBIT = 1,
            BACKGROUND = 2
        };
    }

    namespace models
    {
        enum
        {
            BLACK_HOLE = 0,
            STAR = 1,
            PLANET = 2,
            MOON = 3
        };
    }
    
}


#endif