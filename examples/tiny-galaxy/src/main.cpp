#include "bn_core.h"

#include "tg_definitions.h"
#include "tg_entities.h"
#include "tg_u_orbit.h"
#include "tg_u_scenegraph.h"
#include "tg_u_background.h"


int main()
{
    bn::core::init();

    // create an entity table
    tg::entity_table table;

    // define the available components
    table.add_component<tg::position>(tg::tags::POSITION);
    table.add_component<tg::orbit>(tg::tags::ORBIT);
    table.add_component<tg::sprite>(tg::tags::SPRITE);
    table.add_component<tg::entity>(tg::tags::PARENT);
    
    // attach some updaters to the table
    table.add_updater(new tg::u_orbit(table));
    table.add_updater(new tg::u_scenegraph(table));
    table.add_updater(new tg::u_background());

    // initialize the updaters
    table.init();

    // spawn a galaxy complex
    tg::entities::galaxy(table, 0, 0);

    while (true)
    {
        // Update all the updaters in the table
        table.update();
        bn::core::update();
    }

}