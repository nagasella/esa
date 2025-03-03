#include "bn_core.h"
#include "bn_keypad.h"

#include "esa.h"

#include "cs_entities.h"
#include "cs_definitions.h"
#include "cs_u_movement.h"
#include "cs_u_rotation.h"
#include "cs_u_scaling.h"
#include "cs_u_visibility.h"
#include "cs_u_entity_manager.h"
#include "cs_u_animation.h"

using namespace cs;

int main()
{
    bn::core::init();

    // Define an entity table
    entity_table table;

    // Define all the entity models
    table.fixed.add<models::RED_SQUARE, fields::X>();
    table.fixed.add<models::RED_SQUARE, fields::Y>();
    table.fixed.add<models::RED_SQUARE, fields::VX>();
    table.fixed.add<models::RED_SQUARE, fields::VY>();
    table.enums.add<models::RED_SQUARE, fields::SCALE, fields::SCALE_SZ>();

    table.fixed.add<models::BLUE_SQUARE, fields::X>();
    table.fixed.add<models::BLUE_SQUARE, fields::Y>();
    table.fixed.add<models::BLUE_SQUARE, fields::VX>();
    table.fixed.add<models::BLUE_SQUARE, fields::VY>();
    table.intgs.add<models::BLUE_SQUARE, fields::ANGLE>();
    table.bools.add<models::BLUE_SQUARE, fields::VISIBLE>();

    table.fixed.add<models::YELLOW_SQUARE, fields::X>();
    table.fixed.add<models::YELLOW_SQUARE, fields::Y>();
    table.fixed.add<models::YELLOW_SQUARE, fields::VX>();
    table.fixed.add<models::YELLOW_SQUARE, fields::VY>();
    table.intgs.add<models::YELLOW_SQUARE, fields::ANGLE>();
    table.enums.add<models::YELLOW_SQUARE, fields::SCALE, fields::SCALE_SZ>();

    table.fixed.add<models::FLASHING_SQUARE, fields::X>();
    table.fixed.add<models::FLASHING_SQUARE, fields::Y>();
    table.fixed.add<models::FLASHING_SQUARE, fields::VX>();
    table.fixed.add<models::FLASHING_SQUARE, fields::VY>();
    table.intgs.add<models::FLASHING_SQUARE, fields::ANGLE>();
    table.enums.add<models::FLASHING_SQUARE, fields::SCALE, fields::SCALE_SZ>();
    table.enums.add<models::FLASHING_SQUARE, fields::ANIM_CURR, fields::ANIM_CURR_SZ>();
    table.enums.add<models::FLASHING_SQUARE, fields::ANIM_FIRST, fields::ANIM_FIRST_SZ>();
    table.enums.add<models::FLASHING_SQUARE, fields::ANIM_LAST, fields::ANIM_LAST_SZ>();
    table.enums.add<models::FLASHING_SQUARE, fields::ANIM_TIMER, fields::ANIM_TIMER_SZ>();

    // Set up all the updaters
    table.add_updater(new u_movement(table));
    table.add_updater(new u_rotation(table));
    table.add_updater(new u_visibility(table));
    table.add_updater(new u_scaling(table));
    table.add_updater(new u_animation(table));
    table.add_updater(new u_entity_manager(table));

    // Initialize all the updaters
    table.init();

    // Add 4 entities
    entities::red_square(table);
    entities::blue_square(table);
    entities::yellow_square(table);
    entities::flashing_square(table);

    while (true)
    {
        // Update everything
        table.update();
        bn::core::update();
    }

}