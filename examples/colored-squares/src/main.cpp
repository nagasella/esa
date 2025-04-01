#include "bn_core.h"

#include "esa.h"

#include "cs_entities.h"
#include "cs_definitions.h"
#include "cs_u_movement.h"
#include "cs_u_rotation.h"
#include "cs_u_scaling.h"
#include "cs_u_visibility.h"
#include "cs_u_entity_manager.h"
#include "cs_u_animation.h"
#include "cs_q_rotation.h"

using namespace cs;

int main()
{
    bn::core::init();

    // define an entity table
    entity_table table;

    // set up available components
    table.add_component<position>(tags::POSITION);
    table.add_component<velocity>(tags::VELOCITY);
    table.add_component<sprite>(tags::SPRITE);
    table.add_component<color>(tags::COLOR);
    table.add_component<int>(tags::SCALE);
    table.add_component<int>(tags::ANGLE);
    table.add_component<bool>(tags::VISIBLE);
    table.add_component<uint_set>(tags::ANIM_SET);

    // set up all the updaters, cached queries...
    table.add_updater(new u_movement(table));
    table.add_updater(new u_rotation(table));
    table.add_updater(new u_visibility(table));
    table.add_updater(new u_scaling(table));
    table.add_updater(new u_animation(table));
    table.add_updater(new u_entity_manager(table));
    table.add_query(new q_rotation(table));

    // initialize all the updaters
    table.init();

    // add 4 entities
    entities::red_square(table);
    entities::blue_square(table);
    entities::yellow_square(table);
    entities::flashing_square(table);

    while (true)
    {
        table.update();
        bn::core::update();
    }

}