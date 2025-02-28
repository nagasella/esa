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

using namespace cs;

int main()
{
    bn::core::init();

    // Define an entity table
    cs::entity_table table;

    // Set up all the updaters for the table
    table.add_updater(new u_movement(table));
    table.add_updater(new u_rotation(table));
    table.add_updater(new u_visibility(table));
    table.add_updater(new u_scaling(table));
    table.add_updater(new u_entity_manager(table));

    // Initialize all the updaters
    table.init();

    // Add 3 entities
    entities::red_square(table);
    entities::blue_square(table);
    entities::yellow_square(table);

    while (true)
    {
        // Update everything
        table.update();
        bn::core::update();
    }

}