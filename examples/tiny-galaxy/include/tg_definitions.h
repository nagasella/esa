#ifndef TG_DEFINITIONS_H
#define TG_DEFINITIONS_H

#include "esa.h"

#include "bn_optional.h"
#include "bn_sprite_ptr.h"


/**
 * @brief Here we have the definition of all
 * ESA-related stuff used in the game: parameterization
 * of tables, udpaters, tags, ...
 * 
 */
namespace tg
{

    using entity = esa::entity;
    using entity_table = esa::entity_table<128, 4, 3, 0, 0>;
    using entity_updater = esa::entity_updater<128>;
    using table_updater = esa::table_updater;

    // components
    struct position
    {
        bn::fixed x, y;
    };

    struct orbit
    {
        bn::fixed distance, angle, v_angular;
    };

    using sprite = bn::optional<bn::sprite_ptr>;

    // tags
    namespace tags
    {
        enum
        {
            // components
            POSITION = 0, // position relative to the parent
            ORBIT = 1,
            SPRITE = 2,
            PARENT = 3, // tag of the "parent" element in the uintn_set compoonent

            // updaters
            SCENEGRAPH = 0,
            UPDATE_ORBIT = 1,
            BACKGROUND = 2
        };
    }

    namespace entity_type
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