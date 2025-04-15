#ifndef CS_DEFINITIONS_H
#define CS_DEFINITIONS_H

#include "esa.h"

#include "bn_optional.h"
#include "bn_sprite_ptr.h"

/**
 * @brief Here we have the definition of all
 * ESA-related stuff used in the game: parameterization
 * of tables, udpaters, tags, ...
 * 
 */
namespace cs
{
    // parametrization of an entity table and its updaters
    using entity         = esa::entity;
    using entity_table   = esa::entity_table<128, 8, 6, 1, 0>;
    using uint_set       = esa::uintn_set;

    // components
    struct position
    {
        bn::fixed x, y;
    };

    struct velocity
    {
        bn::fixed x, y;
    };

    using sprite = bn::optional<bn::sprite_ptr>;

    // tags for components, updaters, etc
    namespace tags
    {
        enum
        {
            // components
            POSITION = 0, // custom datatypes
            VELOCITY = 1,
            SPRITE = 2,
            COLOR = 3,
            SCALE = 4, // int
            ANGLE = 5,
            VISIBLE = 6, // bool
            
            ANIM_SET = 7, // uintn set for animation info
            ANIM_CURR = 0,
            ANIM_CURR_SZ = 2,
            ANIM_FIRST = 2,
            ANIM_FIRST_SZ = 2,
            ANIM_LAST = 4,
            ANIM_LAST_SZ = 2,
            ANIM_TIMER = 6,
            ANIM_TIMER_SZ = 4,

            // updaters
            MOVEMENT = 0,
            ROTATION = 1,
            SCALING = 2,
            VISIBILITY = 3,
            ENTITY_MANAGER = 4,
            ANIMATION = 5,

            // queries
            QRY_ROTATION = 0
        };
    }

    // colors
    enum class color
    {
        RED, BLUE, YELLOW, FLASHING
    };
    
}


#endif